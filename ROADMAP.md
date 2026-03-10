# Unit Tests

# Benchmarks

## Execution
- Warmup phase
- Configurable number of samples

## Calibration
- Auto calibration of iterations
- Minimum runtime per sample

## Noise reduction
- CPU affinity
- High scheduler priority
- System load detection
- Multiple samples

## Compiler safety
- Prevent dead code elimination
- Prevent constant folding
- Optional noinline
- Memory barriers
- black_box() function

## Measurement
- High resolution timer
- Baseline measurement
- Avoid measuring setup/teardown

## Statistics
- median
- p95
- p99
- min
- max
- outlier detection

## Cache behavior
- hot cache mode
- cold cache mode

## Environment
- CPU model
- cores
- cache sizes
- kernel version
- CPU governor
- SMT state
- compiler flags
