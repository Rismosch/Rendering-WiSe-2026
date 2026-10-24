pub mod gnuplot;

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
    let seed = Seed::default();
    eprintln!("{:?}", seed);
    let mut rng = Rng::new(seed);

    run_2_2(&mut rng)?;
    run_2_3(&mut rng)?;

    Ok(())
}

fn run_2_2(rng: &mut Rng) -> RisResult<()> {
    const SAMPLE_COUNT: usize = 1000;

    let mut gnuplot = Gnuplot::open()?;

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

        gnuplot.run(format!("set title 'Integral 1'"))?;

        gnuplot.run("set xlabel 'N'")?;
        gnuplot.run("set ylabel '|I_1 - F_N|'")?;

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

        gnuplot.run(format!("set title 'Integral 2'"))?;

        gnuplot.run("set xlabel 'N'")?;
        gnuplot.run("set ylabel '|I_2 - F_N|'")?;

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

        gnuplot.run(format!("set title 'Integral 3'"))?;

        gnuplot.run("set xlabel 'N'")?;
        gnuplot.run("set ylabel '|I_3 - F_N|'")?;

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

    let mut gnuplot = Gnuplot::open()?;

    // f(x)
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
        gnuplot.run("set output 'assignment_2_3_f.svg'")?;

        gnuplot.run(format!(
            "set title 'Rejection Sampling for f(x) with {} samples'",
            SAMPLE_COUNT,
        ))?;
        gnuplot.run("set xlabel 'x'")?;
        gnuplot.run("set ylabel 'y'")?;

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

    // g(x)
    {
        let mut samples = [(0.0, 0.0); SAMPLE_COUNT];
        for (x, y) in samples.iter_mut() {
            *x = rng.next_u128() as f64 / (u128::MAX as f64);
            *y = rng.next_u128() as f64 / (u128::MAX as f64) * 6.0;
        }

        let acceppted = samples.iter()
            .cloned()
            .filter(|&(x, y)| {
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
        gnuplot.run("set output 'assignment_2_3_g.svg'")?;

        gnuplot.run(format!(
            "set title 'Rejection Sampling for g(x) with {} samples'",
            SAMPLE_COUNT,
        ))?;
        gnuplot.run("set xlabel 'x'")?;
        gnuplot.run("set ylabel 'y'")?;

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

    Ok(())
}
