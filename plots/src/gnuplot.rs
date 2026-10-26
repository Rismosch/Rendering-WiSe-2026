use std::io::Write;
use std::process::Child;
use std::process::Command;
use std::process::Stdio;

use ris_error::prelude::*;

#[derive(Debug)]
pub struct Gnuplot {
    process: Child,
    debug: bool,
}

impl Drop for Gnuplot {
    fn drop(&mut self) {
        if let Err(e) = exit_gnuplot(&mut self.process) {
            eprintln!("failed to exit gnuplot: {:?}", e)
        }
    }
}

impl Gnuplot {
    pub fn new() -> RisResult<Self> {
        eprintln!("spawn new gnuplot instance...");
        let process = Command::new("gnuplot")
            .stdin(Stdio::piped())
            .spawn()
            .ris_expect("gnuplot to spawn")?;
        eprintln!("gnuplot is running!");

        let debug = false;
        Ok(Self { process, debug })
    }

    pub fn debug(&mut self) -> &mut bool {
        &mut self.debug
    }

    pub fn run(&mut self, command: impl AsRef<str>) -> RisResult<()> {
        let command = command.as_ref();
        if self.debug {
            eprintln!("gnuplot> {}", command);
        }
        let stdin = self.process.stdin.as_mut().ris_expect("stdin to exist")?;
        writeln!(stdin, "{}", command)?;
        Ok(())
    }
}

fn exit_gnuplot(gnuplot: &mut Child) -> RisResult<()> {
    let stdin = gnuplot.stdin.as_mut().ris_expect("stdin to exist")?;
    writeln!(stdin, "exit")?;
    let timeout = std::time::Duration::from_secs(5);
    let start = std::time::Instant::now();
    loop {
        if let Some(exit_status) = gnuplot.try_wait()? {
            println!("gnuplot exited with {}", exit_status);
            break;
        }

        let now = std::time::Instant::now();
        let diff = now - start;
        if diff > timeout {
            println!("failed to wait for gnuplot to exit in {:?}", timeout);
            break;
        }

        std::thread::yield_now();
    }

    Ok(())
}
