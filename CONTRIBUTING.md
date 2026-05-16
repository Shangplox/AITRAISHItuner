# Contributing to AITRAISHItuner

Thank you for contributing. This document explains how to get started and what standards are expected.

---

## Before you start

- Read [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) — especially the thread model section
- Read [docs/DSP.md](docs/DSP.md) — understand what each DSP module does before changing it
- Open an issue before starting a large feature — discuss scope and design first
- Fork the repo and create a feature branch from `main`

---

## Build and test

```bash
cmake --preset windows-vs2022-debug
cmake --build build/debug --config Debug
ctest --preset windows-vs2022-debug --output-on-failure
```

All tests must pass before submitting a PR.

---

## Code standards

### Real-time safety (critical)

The audio thread in `processBlock` must:

- Never allocate or free heap memory
- Never lock a mutex
- Never do file I/O
- Never call any JUCE Component method
- Never use `DBG()` or `std::cout`
- Never call a blocking function

If your code violates any of these rules, the PR will be rejected.

### Style

- C++20
- PascalCase for classes: `SampleEngine`, `DistortionEngine`
- camelCase for methods and variables: `processBlock`, `m_sampleBuffer`
- `m_` prefix for all member variables
- All parameter IDs as `constexpr const char*` in `Parameters.h`
- No comments that describe what the code does — only comment **why** when it's non-obvious
- No files longer than ~400 lines — split if needed

### Parameter IDs

Parameter IDs are **permanent**. Do not rename them. Do not remove them. If you need to add a parameter, choose a new ID that has never been used and add it with a bumped version hint.

---

## Testing requirements

For any DSP change:
- Add or update a test in `tests/DSP/`
- Test silence → silence
- Test NaN/Inf safety
- Test boundary conditions

For any UI change:
- Describe how to manually verify in the PR description
- Include before/after screenshots if the layout changes

---

## PR checklist

- [ ] All tests pass (`ctest --preset windows-vs2022-release`)
- [ ] No new raw parameter IDs introduced without `Parameters.h` update
- [ ] No real-time thread violations
- [ ] No memory allocation in `processBlock`
- [ ] New DSP modules have tests
- [ ] PR description explains why, not just what
- [ ] Changelog entry added to `CHANGELOG.md`

---

## Reporting bugs

Use the GitHub issue tracker. Include:

1. FL Studio version
2. Windows version
3. Buffer size and sample rate
4. Steps to reproduce
5. Whether the plugin crashes FL Studio or just misbehaves
