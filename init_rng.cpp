#include <random>

using std::uniform_real_distribution;

extern uniform_real_distribution<double> eyeball_vx_distribution;
extern uniform_real_distribution<double> coin_spawn_rate_distribution;

void init_rng() {
  eyeball_vx_distribution = uniform_real_distribution<double>(-4.0, -2.0);
  coin_spawn_rate_distribution = uniform_real_distribution<double>(0.0, 1.0);
}
