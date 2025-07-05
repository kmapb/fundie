#include <cstdio>
#include <yaml-cpp/yaml.h>
#include <vector>
#include <getopt.h>
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

void run_single_simulation(const FundConfig& config, int simulation_num) {
    printf("\n=== Simulation %d ===\n", simulation_num);

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
    printf("distributions: $%'.2fM\n", pb2.distributed() / 1e6);
    printf("    to LPs   : $%'.2fM\n", pb2.distributed_to_lps() / 1e6);
    printf("    to GPs   : $%'.2fM\n", pb2.distributed_to_gps() / 1e6);
    printf("    carried  : $%'.2fM\n", pb2.carried_interest() / 1e6);
    printf("    fees     : $%'.2fM\n", pb2.fees_paid() / 1e6);
    printf("cash returned: $%'.2fM\n", pb2.dry_powder() / 1e6);
}

void print_usage(const char* program_name) {
    printf("Usage: %s [OPTIONS] [config_file]\n", program_name);
    printf("Options:\n");
    printf("  -n, --num-runs=N    Number of simulations to run (default: 1)\n");
    printf("  -h, --help          Show this help message\n");
    printf("\n");
    printf("If config_file is not specified, defaults to 'data/config.yaml'\n");
}

int main(int argc, char** argv) {
    int num_runs = 1;
    const char* config_file = "data/config.yaml";

    // Parse command line arguments
    static struct option long_options[] = {
        {"num-runs", required_argument, 0, 'n'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    int opt;
    int option_index = 0;
    while ((opt = getopt_long(argc, argv, "n:h", long_options, &option_index)) != -1) {
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
            default:
                print_usage(argv[0]);
                return 1;
        }
    }

    // Check for config file argument
    if (optind < argc) {
        config_file = argv[optind];
    }

    printf("yarr ...\n");
    for (int i = 0; i < num_stages; i++) {
        printf("s[%s]: %g\n", stages[i].name, stages[i].p_exit);
    }

    printf("\nRunning %d simulation%s with config: %s\n",
           num_runs, (num_runs == 1) ? "" : "s", config_file);

    // Load configuration from YAML file
    FundConfig config = load_config(config_file);

    // Run simulations
    for (int i = 1; i <= num_runs; i++) {
        run_single_simulation(config, i);
    }

    if (num_runs > 1) {
        printf("\n=== Completed %d simulations ===\n", num_runs);
    }

    return 0;
}