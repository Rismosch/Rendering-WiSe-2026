# Assignment 02

This repo contains the code that generates the plots for Assignment 02 for the class Rendering WiSe 2026.

# Requirements

- Rust: https://rust-lang.org/learn/get-started/
- Gnuplot: http://www.gnuplot.info/download.html

To verify that you have installed everything correctly, run the following commands:

    rustc --version
    cargo --version
    gnuplot --version

Without Rust/Cargo, you wont be able to compile the repo. And without gnuplot, you will fail to generate plots, which probably results in runtime errors.

# How to run

In the root of this repo, run:

    cargo run

It will produce several svg files at the root of this repo.

# Where is the code located?

The main logic is found in `./src/main.rs`. A simple gnuplot wrapper is foud in `./src/gnuplot`. The directory `./crates` includes an RNG based on [PCG](https://www.pcg-random.org/) and a quality-of-life error utility, both lifted from my [game engine](https://github.com/Rismosch/ris_engine).
