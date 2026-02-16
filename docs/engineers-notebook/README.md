# Engineer's Notebook

This directory contains design decisions, trade-off analyses, and educational background material for the DEFCON SSTV Badge project.

**These documents explain *why* the design is what it is.** The main documentation files describe *what* the design is.

---

## Contents

### Design Decisions

- **[electrical-design-decisions.md](electrical-design-decisions.md)** — Power system redesign rationale, buck-boost regulator selection, audio codec selection (PCM5102A vs WM8960), carrier board rationale, MCU variant selection (RP2350B), display/camera interface decisions, alternate part options

### Educational Background

- **[audio-concepts-explained.md](audio-concepts-explained.md)** — Beginner-friendly explanation of filtering, bias, DC blocking, anti-aliasing, and other audio concepts used in the badge's RX/TX circuits

---

## How to Use This Notebook

1. **New contributors**: Read these documents to understand *why* design decisions were made
2. **Design changes**: Document new decisions here before implementing them
3. **Troubleshooting**: Understanding the rationale helps diagnose issues

---

*Last updated: 2026-02-15*
