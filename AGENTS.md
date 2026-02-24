# Fundie: VC Portfolio Simulator

This repository contains a C++ Monte Carlo-style simulator for a venture capital fund.

## Build and test

- Configure: `meson setup build` (first time)
- Compile: `meson compile -C build`
- Run tests: `meson test -C build`
- Run simulation: `./build/sim data/config.yaml`
- CSV output: `./build/sim -c -n 1000 -j 8 data/config.yaml`

## Code map

- `main.cpp`: CLI, YAML config loading, parallel run orchestration, output formatting.
- `fund.h` / `fund.cpp`: fund cash flows, fees, follow-ons, stage stepping, distribution waterfall.
- `stage.h` / `stage.cpp`: stage definitions and stochastic stage transitions.
- `asset.h`: company valuation/share-count model and dilution mechanics.
- `position.h`: ownership/cost/value aggregation across holdings.
- `round.h` / `round.cpp`: pro-rata helper math (used in tests/utilities).
- `data/*.yaml`: example portfolio and fund parameter configs.
- `test/*.cpp`: unit tests for asset/position/stage/fund behavior.

## Input model

The simulator expects:

- Fund terms: `lp_commitments`, `gp_commitments`, `carry`, `carry_hurdle`, `fees`.
- Initial investments: company name, valuation, check size, and an investment date.

## Modeling assumptions

The assumptions below reflect current code behavior (not idealized VC economics):

- Capital base:
  - Fund size is `lp_commitments + gp_commitments`.
  - All dollars are nominal (no discounting, inflation, or FX).
- Portfolio construction:
  - One `Asset` per company; positions are keyed by company name.
  - Each asset starts with a fixed `10,000,000` outstanding shares.
  - Ownership is represented via shares; position value is ownership times company value.
- Initial deployment:
  - `deploy` buys newly issued shares from the company and increases company value by the invested amount.
  - Config field `post_money_valuation` is consumed directly as the starting company value in this issuance model.
- Time progression:
  - The simulation steps through fixed stages (`Seed` -> ... -> `IPO`) using a global stage index for all active companies.
  - Investment dates are stored but do not affect pacing or stage timing.
- Stage transitions:
  - Each tick, a company can die, exit, or continue using per-stage probabilities.
  - If not dead, company value is multiplied by stage growth factor.
  - A hardcoded financing event adds `20%` new money at each stage transition.
  - Option-pool expansion adds shares per stage (`option_pool`) and dilutes existing holders.
- Follow-on reserve strategy:
  - On `CONTINUE`, the fund attempts pro-rata follow-ons:
    `to_deploy = min(dry_powder, asset_value * old_ownership * stage.dilution)`.
  - Follow-ons stop when dry powder is exhausted.
- Fees:
  - Management fees are charged per tick as `fees * commitments`, capped by remaining dry powder.
  - Fees reduce dry powder directly; there is no separate fee reserve account.
- Distribution logic:
  - Waterfall is deal-by-deal (American style) at each company exit.
  - Before hurdle: proceeds are split pro-rata between LPs and GPs by commitment ratio.
  - After hurdle: `carry` percent is skimmed to carried interest; remainder is split pro-rata LP/GP.
  - Hurdle is measured as total distributed divided by total commitments, not IRR/preferred-return math.
  - No clawback, GP catch-up, recycling, taxes, or reserve constraints beyond dry powder.
- Outcomes:
  - Dead companies go to zero and distribute nothing.
  - IPO stage has `p_exit = 1.0`, so surviving companies eventually exit.

## Known implementation caveats

- Stage advancement currently increments in both `tick()` and `run_to_completion()`, effectively skipping every other stage before saturating at `IPO`.
- Randomness uses process-global `random()` and no explicit seeding in `main`, which can affect reproducibility and thread interaction.
- `Position` supports SAFE holdings, but the fund simulation path currently deploys only share holdings.
