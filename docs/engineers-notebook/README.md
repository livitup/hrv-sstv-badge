# Engineer's Notebook

This directory contains design decisions, trade-off analyses, and educational background material for the DEFCON SSTV Badge project.

**These documents explain *why* the design is what it is.** The main documentation files describe *what* the design is.

---

## Contents

### Design Decisions

- **[electrical-design-decisions.md](electrical-design-decisions.md)** — Power system redesign rationale, buck-boost regulator selection, audio codec selection (PCM5102A vs WM8960), carrier board rationale, MCU variant selection (RP2350B), display/camera interface decisions, alternate part options

### Trade-Off Analyses

- **[rda1846-vs-sa818-analysis.md](rda1846-vs-sa818-analysis.md)** — Detailed comparison of using the bare RDA1846S transceiver IC vs the SA818 module (which contains the RDA1846S internally). Covers engineering effort, cost, SSTV audio compatibility, assembly, sourcing risk, and carrier board implications. **Decision: Stay with SA818.**

### Educational Background

- **[audio-concepts-explained.md](audio-concepts-explained.md)** — Beginner-friendly explanation of filtering, bias, DC blocking, anti-aliasing, and other audio concepts used in the badge's RX/TX circuits

---

## How to Use This Notebook

1. **New contributors**: Read these documents to understand *why* design decisions were made
2. **Design changes**: Document new decisions here before implementing them
3. **Troubleshooting**: Understanding the rationale helps diagnose issues

---

*Last updated: 2026-02-16*
