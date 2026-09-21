# 598APE-HW1

This repository contains code for homework 1 of 598APE.

In particular, this repository is an implementation of a Raytracer.

## Build with Docker

Run the following command. Make sure you have Docker installed on your machine.

It builds the required compiler toolchain into a Docker image.

```bash
docker build -t "598ape-ringo-leo" -f docker/Dockerfile .
```

## Run with Docker

```bash
docker run -it -v ".:/workspace" -t 598ape-ringo-leo bash
```

After this, the project root will be in `/workspace`.

## Checkout and Compile Programs

We develop our code on the `main` branch. However, it contains all of the CI code and Makefile changes in the middle of the commit history. For better testing, we reorder the commits and create a new branch called `final`, so that all miscellaneous changes are at the bottom of the tree.

First, checkout the `final` branch.

```bash
git checkout final
```

If Git is unhappy about modifying the Git repository inside the container, run the following command:

```bash
git config --global --add safe.directory /workspace
```

## Test a Specific Optimization

To test the speedup of a specific optimization, we first test the commit where the optimization is introduced and then test `HEAD^`.

The commit hash can be found using `git log`.

The testing procedure is:

```bash
git checkout <HASH>
make clean
make
# Run your tests.

git checkout HEAD^
make clean
make
# Run your tests.
```

Here are the possible test cases. We strongly recommend reproducing Elephant only after the BVH patch.

```bash
# Test PianoRoom
./main.exe -i inputs/pianoroom.ray --ppm -o output/pianoroom.ppm -H 500 -W 500

# Test Globe
./main.exe -i inputs/globe.ray --ppm -a inputs/globe.animate --movie -F 24

# Test Sphere
./main.exe -i inputs/elephant.ray --ppm -a inputs/elephant.animate --movie -F 24 -W 100 -H 100 -o output/sphere.mp4

# Test Elephant
./main.exe -i inputs/elephant_real.ray --ppm -a inputs/elephant.animate --movie -F 24 -W 100 -H 100 -o output/elephant.mp4
```

Alternatively, you can run `./scripts/performance-test.sh` directly.

This script runs the previous three or four scenes (`elephant_real` was added right before the BVH patch). Each scene is run three times, and the average value is reported.
