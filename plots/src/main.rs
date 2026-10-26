pub mod gnuplot;

use std::f64::consts::FRAC_PI_2;
use std::f64::consts::PI;

use ris_error::prelude::*;
use ris_rng::rng::Rng;
use ris_rng::rng::Seed;

use crate::gnuplot::Gnuplot;

fn main() -> Result<std::process::ExitCode, String> {
    run_program().map_err(|e| {
        eprintln!("{:?}", e);
        format!("{}", e)
    })?;

    Ok(0.into())
}

fn run_program() -> RisResult<()> {
    let seed = Seed::new();
    eprintln!("{:?}", seed);
    let mut rng = Rng::new(seed);

    run_2_2(&mut rng)?;
    run_2_3(&mut rng)?;

    Ok(())
}

fn run_2_2(rng: &mut Rng) -> RisResult<()> {
    const SAMPLE_COUNT: usize = 1000;

    let mut gnuplot = Gnuplot::new()?;

    let mut uniform_distribution = [0.0; SAMPLE_COUNT];
    for x in uniform_distribution.iter_mut() {
        *x = rng.next_u128() as f64 / (u128::MAX as f64);
    }

    // F_1
    {
        const I_1: f64 = 2.0;
        let mut error_1 = [0.0; SAMPLE_COUNT];

        for i in 0..SAMPLE_COUNT {
            #[allow(non_snake_case)]
            let N = i + 1;
            let mut sum = 0.0;
            for &x in uniform_distribution.iter().take(N) {
                let x = x * PI;
                let f = f64::sin(x);
                let p = 1.0 / PI;
                sum += f / p;
            }

            #[allow(non_snake_case)]
            let F = 1.0 / (N as f64) * sum;
            error_1[i] = f64::abs(I_1 - F);
        }

        gnuplot.run("set terminal svg enhanced font 'DejaVu Sans,16'")?;
        gnuplot.run("set output 'assignment_2_2_integral_1.svg'")?;

        gnuplot.run(format!("set title '$\\\\int\\_0\\^\\\\pi \\\\sin(x)\\\\,dx = 2$'"))?;

        gnuplot.run("set xlabel '$N$'")?;
        gnuplot.run("set ylabel '$|I\\_1 - F\\_N|$'")?;

        gnuplot.run("plot '-' using 1:2 with lines title 'Absolute Error'")?;
        for (i, &error) in error_1.iter().enumerate() {
            gnuplot.run(format!("{} {}", i, error))?;
        }
        gnuplot.run("e")?;
    }

    // F_2
    {
        const I_2: f64 = 124.0;
        let mut error_2 = [0.0; SAMPLE_COUNT];

        for i in 0..SAMPLE_COUNT {
            #[allow(non_snake_case)]
            let N = i + 1;
            let mut sum = 0.0;
            for &x in uniform_distribution.iter().take(N) {
                let x = x * 2.0 + 1.0;
                let f = 4.0 * x * x * x + 6.0 * x * x - 3.0 * x + 2.0;
                let p = 1.0 / (3.0 - 1.0);
                sum += f / p;
            }

            #[allow(non_snake_case)]
            let F = 1.0 / (N as f64) * sum;
            error_2[i] = f64::abs(I_2 - F);
        }

        gnuplot.run("set terminal svg enhanced font 'DejaVu Sans,16'")?;
        gnuplot.run("set output 'assignment_2_2_integral_2.svg'")?;

        gnuplot.run(format!("set title '$\\\\int\\_1\\^3 4x\\^3+6x\\^2-3x+2\\\\,dx = 124$'"))?;

        gnuplot.run("set xlabel '$N$'")?;
        gnuplot.run("set ylabel '$|I\\_2 - F\\_N|$'")?;

        gnuplot.run("plot '-' using 1:2 with lines title 'Absolute Error'")?;
        for (i, &error) in error_2.iter().enumerate() {
            gnuplot.run(format!("{} {}", i, error))?;
        }
        gnuplot.run("e")?;
    }

    // F_3
    {
        #[allow(non_snake_case)]
        let I_3: f64 = 0.5 - 0.5 * f64::cos(1.0) * f64::cos(1.0);
        let mut error_3 = [0.0; SAMPLE_COUNT];

        for i in 0..SAMPLE_COUNT {
            #[allow(non_snake_case)]
            let N = i + 1;
            let mut sum = 0.0;
            for &x in uniform_distribution.iter().take(N) {
                let f = f64::sin(x) * f64::cos(x);
                let p = 1.0;
                sum += f / p;
            }

            #[allow(non_snake_case)]
            let F = 1.0 / (N as f64) * sum;
            error_3[i] = f64::abs(I_3 - F);
        }

        gnuplot.run("set terminal svg enhanced font 'DejaVu Sans,16'")?;
        gnuplot.run("set output 'assignment_2_2_integral_3.svg'")?;

        gnuplot.run(format!("set title '$\\\\int\\_0\\^1 sin(x)cos(x)\\\\,dx \\\\approx 0.354$'"))?;

        gnuplot.run("set xlabel '$N$'")?;
        gnuplot.run("set ylabel '$|I\\_3 - F\\_N|$'")?;

        gnuplot.run("plot '-' using 1:2 with lines title 'Absolute Error'")?;
        for (i, &error) in error_3.iter().enumerate() {
            gnuplot.run(format!("{} {}", i, error))?;
        }
        gnuplot.run("e")?;
    }

    Ok(())
}

fn run_2_3(rng: &mut Rng) -> RisResult<()> {
    const SAMPLE_COUNT: usize = 1000;

    let mut gnuplot = Gnuplot::new()?;

    // f(x) - inversion
    {
        let mut samples = [0.0; SAMPLE_COUNT];
        for x in samples.iter_mut() {
            *x = rng.next_u128() as f64 / (u128::MAX as f64);
        }

        let mapped = samples
            .iter()
            .copied()
            .map(|x| {
                (x + 2.0) / 5.0
            })
            .collect::<Vec<_>>();

        let min = 0.4;
        let max = 0.6;
        let steps = 10;
        let step = (max - min) / steps as f64;
        let mut buckets = Vec::new();
        for i in 0..steps {
            let lower = step * i as f64 + min;
            let upper = lower + step;

            let mut count = 0;
            for &x in mapped.iter() {
                if lower <= x && x < upper {
                    count += 1;
                }
            }

            let bucket = 5.0 * steps as f64 * count as f64 / SAMPLE_COUNT as f64;
            buckets.push((lower, bucket));
        }

        *gnuplot.debug() = false;

        gnuplot.run("set terminal svg enhanced font 'DejaVu Sans,16'")?;
        gnuplot.run("set output 'assignment_2_3_f_inverse.svg'")?;

        gnuplot.run(format!("set title 'Inverse Transform Sampling for $f(x)$'"))?;
        gnuplot.run("set xlabel '$x$'")?;
        gnuplot.run("set ylabel 'Frequency Density'")?;

        gnuplot.run("set key noopaque")?;
        gnuplot.run("set xrange [0:1]")?;
        gnuplot.run("set yrange [0:10]")?;
        gnuplot.run("f(x) = (0.4 <= x && x <= 0.6) ? 5 : 0")?;

        gnuplot.run("set style fill solid 0.65 border rgb '#23b31d'")?;

        gnuplot.run(format!(
            "plot '-' using ($1+{}/2):2 with boxes lc rgb '#23b31d' title 'Generated Data', \\",
            step,
        ))?;
        gnuplot.run("f(x) lc rgb '#2c2be6' title 'Expected PDF'")?;

        for (x, bucket) in buckets {
            gnuplot.run(format!("{} {}", x, bucket))?;
        }

        gnuplot.run("e")?;
        gnuplot.run("set yrange [*:*]")?;

        *gnuplot.debug() = false;
    }

    // f(x) - rejection
    {
        let mut samples = [(0.0, 0.0); SAMPLE_COUNT];
        for (x, y) in samples.iter_mut() {
            *x = rng.next_u128() as f64 / (u128::MAX as f64);
            *y = rng.next_u128() as f64 / (u128::MAX as f64) * 10.0;
        }

        let acceppted = samples.iter()
            .cloned()
            .filter(|&(x, y)| {
                if x < 0.4 {
                    return false;
                }

                if x > 0.6 {
                    return false;
                }

                if y > 5.0 {
                    return false;
                }

                return true;
            })
            .collect::<Vec<_>>();

        let rejected = samples.iter()
            .cloned()
            .filter(|x| {
                !acceppted.contains(x)
            })
            .collect::<Vec<_>>();

        gnuplot.run("set terminal svg enhanced font 'DejaVu Sans,16'")?;
        gnuplot.run("set output 'assignment_2_3_f_reject.svg'")?;

        gnuplot.run(format!(
            "set title 'Rejection Sampling for $f(x)$ with {} samples'",
            SAMPLE_COUNT,
        ))?;
        gnuplot.run("set xlabel '$x$'")?;
        gnuplot.run("set ylabel '$y$'")?;

        gnuplot.run("set key opaque")?;
        gnuplot.run("set xrange [0:1]")?;
        gnuplot.run("f(x) = (0.4 <= x && x <= 0.6) ? 5 : 0")?;

        gnuplot.run("plot f(x) lc rgb 'blue', \\")?;
        gnuplot.run("'-' using 1:2 with points pt 1 lc rgb 'green' title 'Accepted Samples', \\")?;
        gnuplot.run("'-' using 1:2 with points pt 2 lc rgb 'red' title 'Rejected Samples'")?;

        for &(x, y) in acceppted.iter() {
            gnuplot.run(format!("{} {}", x, y))?;
        }
        gnuplot.run("e")?;

        for &(x, y) in rejected.iter() {
            gnuplot.run(format!("{} {}", x, y))?;
        }
        gnuplot.run("e")?;
    }

    // g(x) - inversion
    {
        let mut samples = [0.0; SAMPLE_COUNT];
        for x in samples.iter_mut() {
            *x = rng.next_u128() as f64 / (u128::MAX as f64);
        }

        let mapped = samples
            .iter()
            .copied()
            .map(|x| {
                use statrs::function::erf::erf;
                use statrs::function::erf::erf_inv;

                let o = 0.1;
                let u = 0.5;
                f64::sqrt(2.0) * o * erf_inv(2.0 * x + erf(- u / (f64::sqrt(2.0) * o))) + u
            })
            .collect::<Vec<_>>();

        let min = 0.0;
        let max = 1.0;
        let steps = 20;
        let step = 1.0 / steps as f64;
        let mut buckets = Vec::new();
        for i in 0..steps {
            let lower = (max - min) * step * i as f64;
            let upper = lower + step;

            let mut count = 0;
            for &x in mapped.iter() {
                if lower <= x && x < upper {
                    count += 1;
                }
            }

            let bucket = steps as f64 * count as f64 / SAMPLE_COUNT as f64;
            buckets.push((lower, bucket));
        }

        gnuplot.run("set terminal svg enhanced font 'DejaVu Sans,16'")?;
        gnuplot.run("set output 'assignment_2_3_g_inverse.svg'")?;

        gnuplot.run(format!("set title 'Inverse Transform Sampling for $h(x)$'"))?;
        gnuplot.run("set xlabel '$x$'")?;
        gnuplot.run("set ylabel 'Frequency Density'")?;

        gnuplot.run("set key noopaque")?;
        gnuplot.run("set xrange [0:1]")?;
        gnuplot.run("set yrange [0:6]")?;
        gnuplot.run("g(x) = 1 / (0.1 * sqrt(2 * pi)) * exp(- ((x - 0.5) * (x - 0.5)) / (2 * 0.1 * 0.1))")?;

        gnuplot.run("set style fill solid 0.65 border rgb '#23b31d'")?;

        gnuplot.run(format!(
            "plot '-' using ($1+{}/2):2 with boxes lc rgb '#23b31d' title 'Generated Data', \\",
            step,
        ))?;
        gnuplot.run("g(x) lc rgb '#2c2be6' title 'Expected PDF'")?;

        for (x, bucket) in buckets {
            gnuplot.run(format!("{} {}", x, bucket))?;
        }

        gnuplot.run("e")?;
        gnuplot.run("set yrange [*:*]")?;
    }

    // g(x) - rejection
    {
        let mut samples = [(0.0, 0.0); SAMPLE_COUNT];
        for (x, y) in samples.iter_mut() {
            *x = rng.next_u128() as f64 / (u128::MAX as f64);
            *y = rng.next_u128() as f64 / (u128::MAX as f64) * 6.0;
        }

        let acceppted = samples.iter()
            .cloned()
            .filter(|&(x, y)| {
                if x < 0.0 {
                    return false;
                }

                if x > 1.0 {
                    return false;
                }

                let o = 0.1;
                let u = 0.5;
                let g_x = 1.0 / (o * f64::sqrt(2.0 * PI)) * f64::exp(- ((x - u) * (x - u)) / (2.0 * o * o));
                y <= g_x
            })
            .collect::<Vec<_>>();

        let rejected = samples.iter()
            .cloned()
            .filter(|x| {
                !acceppted.contains(x)
            })
            .collect::<Vec<_>>();

        gnuplot.run("set terminal svg enhanced font 'DejaVu Sans,16'")?;
        gnuplot.run("set output 'assignment_2_3_g_reject.svg'")?;

        gnuplot.run(format!(
            "set title 'Rejection Sampling for $g(x)$ with {} samples'",
            SAMPLE_COUNT,
        ))?;
        gnuplot.run("set xlabel '$x$'")?;
        gnuplot.run("set ylabel '$y$'")?;

        gnuplot.run("set key opaque")?;
        gnuplot.run("set xrange [0:1]")?;
        gnuplot.run("g(x) = 1 / (0.1 * sqrt(2 * pi)) * exp(- ((x - 0.5) * (x - 0.5)) / (2 * 0.1 * 0.1))")?;

        gnuplot.run("plot g(x) lc rgb 'blue', \\")?;
        gnuplot.run("'-' using 1:2 with points pt 1 lc rgb 'green' title 'Accepted Samples', \\")?;
        gnuplot.run("'-' using 1:2 with points pt 2 lc rgb 'red' title 'Rejected Samples'")?;

        for &(x, y) in acceppted.iter() {
            gnuplot.run(format!("{} {}", x, y))?;
        }
        gnuplot.run("e")?;

        for &(x, y) in rejected.iter() {
            gnuplot.run(format!("{} {}", x, y))?;
        }
        gnuplot.run("e")?;
    }

    // h(x) - inversion
    {
        let mut samples = [0.0; SAMPLE_COUNT];
        for x in samples.iter_mut() {
            *x = rng.next_u128() as f64 / (u128::MAX as f64);
        }

        let mapped = samples
            .iter()
            .copied()
            .map(|x| {
                2.0 / PI * f64::acos(-x+1.0)
            })
            .collect::<Vec<_>>();

        let min = 0.0;
        let max = 1.0;
        let steps = 10;
        let step = 1.0 / steps as f64;
        let mut buckets = Vec::new();
        for i in 0..steps {
            let lower = (max - min) * step * i as f64;
            let upper = lower + step;

            let mut count = 0;
            for &x in mapped.iter() {
                if lower <= x && x < upper {
                    count += 1;
                }
            }

            let bucket = steps as f64 * count as f64 / SAMPLE_COUNT as f64;
            buckets.push((lower, bucket));
        }

        gnuplot.run("set terminal svg enhanced font 'DejaVu Sans,16'")?;
        gnuplot.run("set output 'assignment_2_3_h_inverse.svg'")?;

        gnuplot.run(format!("set title 'Inverse Transform Sampling for $h(x)$'"))?;
        gnuplot.run("set xlabel '$x$'")?;
        gnuplot.run("set ylabel 'Frequency Density'")?;

        gnuplot.run("set key noopaque")?;
        gnuplot.run("set xrange [0:2]")?;
        gnuplot.run("h(x) = (0 <= x && x <= 1) ? pi / 2 * sin(pi / 2 * x) : 0")?;

        gnuplot.run("set style fill solid 0.65 border rgb '#23b31d'")?;

        gnuplot.run(format!(
            "plot '-' using ($1+{}/2):2 with boxes lc rgb '#23b31d' title 'Generated Data', \\",
            step,
        ))?;
        gnuplot.run("h(x) lc rgb '#2c2be6' title 'Expected PDF'")?;

        for (x, bucket) in buckets {
            gnuplot.run(format!("{} {}", x, bucket))?;
        }

        gnuplot.run("e")?;
    }

    // h(x) - rejection
    {
        let mut samples = [(0.0, 0.0); SAMPLE_COUNT];
        for (x, y) in samples.iter_mut() {
            *x = rng.next_u128() as f64 / (u128::MAX as f64) * 2.0;
            *y = rng.next_u128() as f64 / (u128::MAX as f64) * 2.5;
        }

        let acceppted = samples.iter()
            .cloned()
            .filter(|&(x, y)| {
                if x < 0.0 {
                    return false;
                }

                if x > 1.0 {
                    return false;
                }

                let h_x = FRAC_PI_2 * f64::sin(FRAC_PI_2 * x);
                y <= h_x
            })
            .collect::<Vec<_>>();

        let rejected = samples.iter()
            .cloned()
            .filter(|x| {
                !acceppted.contains(x)
            })
            .collect::<Vec<_>>();

        gnuplot.run("set terminal svg enhanced font 'DejaVu Sans,16'")?;
        gnuplot.run("set output 'assignment_2_3_h_reject.svg'")?;

        gnuplot.run(format!(
            "set title 'Rejection Sampling for $h(x)$ with {} samples'",
            SAMPLE_COUNT,
        ))?;
        gnuplot.run("set xlabel '$x$'")?;
        gnuplot.run("set ylabel '$y$'")?;

        gnuplot.run("set key opaque")?;
        gnuplot.run("set xrange [0:2]")?;
        gnuplot.run("h(x) = (0 <= x && x <= 1) ? pi / 2 * sin(pi / 2 * x) : 0")?;

        gnuplot.run("plot h(x) lc rgb 'blue', \\")?;
        gnuplot.run("'-' using 1:2 with points pt 1 lc rgb 'green' title 'Accepted Samples', \\")?;
        gnuplot.run("'-' using 1:2 with points pt 2 lc rgb 'red' title 'Rejected Samples'")?;

        for &(x, y) in acceppted.iter() {
            gnuplot.run(format!("{} {}", x, y))?;
        }
        gnuplot.run("e")?;

        for &(x, y) in rejected.iter() {
            gnuplot.run(format!("{} {}", x, y))?;
        }
        gnuplot.run("e")?;
    }


    Ok(())
}
