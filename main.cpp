#include <cstdio>
#include <yaml-cpp/yaml.h>
#include <vector>
#include <getopt.h>
#include <thread>
#include <cstdlib>
#include "stage.h"
#include "fund.h"
#include "debug.h"

struct InvestmentConfig {
    std::string company_name;
    double post_money_valuation;
    double initial_investment;
    int year, month, day;
};

struct FundConfig {
    double lp_commitments;
    double gp_commitments;
    double carry;
    double carry_hurdle;
    double fees;
    std::vector<InvestmentConfig> investments;
};

FundConfig load_config(const std::string& filename) {
    YAML::Node config = YAML::LoadFile(filename);

    FundConfig fund_config;

    // Load fund parameters
    auto fund_node = config["fund"];
    fund_config.lp_commitments = fund_node["lp_commitments"].as<double>();
    fund_config.gp_commitments = fund_node["gp_commitments"].as<double>();
    fund_config.carry = fund_node["carry"].as<double>();
    fund_config.carry_hurdle = fund_node["carry_hurdle"].as<double>();
    fund_config.fees = fund_node["fees"].as<double>();

    // Load investments
    auto investments_node = config["investments"];
    for (const auto& investment : investments_node) {
        InvestmentConfig inv_config;
        inv_config.company_name = investment["company_name"].as<std::string>();
        inv_config.post_money_valuation = investment["post_money_valuation"].as<double>();
        inv_config.initial_investment = investment["initial_investment"].as<double>();

        auto date_node = investment["investment_date"];
        inv_config.year = date_node["year"].as<int>();
        inv_config.month = date_node["month"].as<int>();
        inv_config.day = date_node["day"].as<int>();

        fund_config.investments.push_back(inv_config);
    }

    return fund_config;
}

struct SimulationResult {
    double distributions;
    double distributions_to_lps;
    double distributions_to_gps;
    double carried_interest;
    double fees_paid;
    double dry_powder;
};

enum output_format {
    CSV,
    TEXT,
};

void print_simulation_result_header(output_format format) {
    if (format == TEXT) {
        return;
    }
    assert(format == CSV);
    printf("distributions,distributions_to_lps,distributions_to_gps,carried_interest,fees_paid,dry_powder\n");
}

void print_results(const SimulationResult& result, output_format format) {
    if (format == TEXT) {
        printf("distributions: $%'.2fM\n", result.distributions / 1e6);
        printf("    to LPs   : $%'.2fM\n", result.distributions_to_lps / 1e6);
        printf(".   to GPs   : $%'.2fM\n", result.distributions_to_gps / 1e6);
        printf("    carried  : $%'.2fM\n", result.carried_interest / 1e6);
        printf("    fees     : $%'.2fM\n", result.fees_paid / 1e6);
        printf("cash returned: $%'.2fM\n", result.dry_powder / 1e6);
        return;
    }
    assert(format == CSV);
    printf("%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n",
           result.distributions,
           result.distributions_to_lps,
           result.distributions_to_gps,
           result.carried_interest,
           result.fees_paid,
           result.dry_powder);
}

 SimulationResult
 run_single_simulation(const FundConfig& config) {
    // Create fund with loaded parameters
    Fund pb2 { config.lp_commitments, config.gp_commitments, config.carry, config.carry_hurdle, config.fees };

    // Process each investment from config. We need a root-of-liveness
    // for assets separate from the stack, since they have an independent
    // identity from the fund or positions in them.
    std::vector<Asset> assets;
    // Make it safe to hold onto pointers into assets.
    assets.reserve(config.investments.size());
    for (const auto& inv : config.investments) {
        Asset& asset = assets.emplace_back(inv.company_name.c_str(), inv.post_money_valuation);
        dbg(harness, ("putting ref asset %p into container\n", (void*)&asset));
        pb2.deploy(asset, YMD(inv.year, inv.month, inv.day), inv.initial_investment);

        auto& pos = pb2.get_position(asset);
        assert(&pos.underlying_asset() == &asset);
        dbg(harness, ("Company            : %s\n", inv.company_name.c_str()));
        dbg(harness, ("  ownership        : %3g%%\n", 100.0 * pos.ownership()));
        dbg(harness, ("  company valuation: $%.2gM\n", asset.value() / 1e6));
    }

    pb2.run_to_completion();
    return {
        pb2.distributed(),
        pb2.distributed_to_lps(),
        pb2.distributed_to_gps(),
        pb2.carried_interest(),
        pb2.fees_paid(),
        pb2.dry_powder(),
    };
}

void print_usage(const char* program_name) {
    printf("Usage: %s [OPTIONS] [config_file]\n", program_name);
    printf("Options:\n");
    printf("  -n, --num-runs=N    Number of simulations to run (default: 1)\n");
    printf("  -c, --csv           Output results in CSV format\n");
    printf("  -j, --jobs=N        Thread-level parallelism\n");
    printf("  -h, --help          Show this help message\n");
    printf("\n");
    printf("If config_file is not specified, defaults to 'data/config.yaml'\n");
}

int main(int argc, char** argv) {
    int num_runs = 1;
    int jobs = ::sysconf(_SC_NPROCESSORS_ONLN);
    const char* config_file = "data/config.yaml";
    auto output_format = TEXT;

    // Parse command line arguments
    static struct option long_options[] = {
        {"num-runs", required_argument, 0, 'n'},
        {"jobs", required_argument, 0, 'j'},
        {"help", no_argument, 0, 'h'},
        {"csv", no_argument, 0, 'c'},
        {0, 0, 0, 0}
    };

    int opt;
    int option_index = 0;
    while ((opt = getopt_long(argc, argv, "n:j:ch", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'n':
                num_runs = atoi(optarg);
                if (num_runs <= 0) {
                    fprintf(stderr, "Error: Number of runs must be positive\n");
                    return 1;
                }
                break;
            case 'h':
                print_usage(argv[0]);
                return 0;
            case 'c':
                output_format = CSV;
                break;
            case 'j':
                jobs = atoi(optarg);
                if (jobs <= 0) {
                    fprintf(stderr, "Error: Number of jobs must be positive\n");
                    return 1;
                }
                break;
            default:
                print_usage(argv[0]);
                return 1;
        }
    }

    // Check for config file argument
    if (optind < argc) {
        config_file = argv[optind];
    }

    // Load configuration from YAML file
    FundConfig config = load_config(config_file);

    print_simulation_result_header(output_format);

    // Run simulations
    std::vector<SimulationResult> results;
    results.resize(num_runs);
    std::vector<std::thread> threads;
    for (int i = 0; i < jobs; i++) {
        int threads_per_job = std::ceil(1.0 * num_runs / jobs);
        threads.emplace_back([i, threads_per_job, num_runs, &results, &config]() {
            auto my_start = i * threads_per_job;
            auto my_end = std::min(my_start + threads_per_job, num_runs);
            for (auto j = my_start; j < my_end; j++) {
                results[j] = run_single_simulation(config);
            }
        });
    }
            
    for (auto& t: threads) {
        t.join();
    }
    for (const auto& result: results) {
        print_results(result, output_format);
    }
    return 0;
}