# Changelog

All notable changes to CognitaOS will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [2.0.0] — Prometheus Release

### Added

- **Differentiable Kernel** — The kernel is now a differentiable computation graph with continuously optimized weights
- **Attention Scheduler** — Single-pass attention mechanism replaces traditional scheduling algorithms
- **Attention Field Memory** — Memory access is now an attention query; page faults replaced by relevance misses
- **Holographic Storage** — Erasure-coded semantic sharding with self-organizing knowledge graph
- **Intent Compiler** — Seven-stage pipeline: Lexical Attention → Semantic Parsing → Goal Decomposition → Capability Resolution → Resource Planning → Verification → Code Generation
- **CognitaIR** — Typed, capability-annotated intermediate representation
- **Agent Lifecycle** — Agents can fork, merge, sleep, and dream
- **Capability Web** — Typed, schema-validated, capability-secured functions replace applications
- **Photonic Interconnect** — Silicon photonics support in the HAL
- **Quantum Coprocessor** — QPU as first-class scheduling target with hybrid runtime
- **Kernel Evolution** — Self-modifying kernel with formal verification and shadow deployment
- **Quantum-Native Security** — QKD for inter-crystal communication, post-quantum cryptography for persistence
- **Temporal Resource Fabric** — Multi-horizon scheduling across nanosecond to minute timescales
- **Symbolic Reasoning Core** — SMT solvers, theorem provers, and model checkers for proof obligations

### Changed

- Replaced POSIX syscalls with single `cog_intent()` API
- Replaced file system with semantic storage engine
- Replaced process model with agent streams
- Replaced address-based memory with attention field

### Removed

- File descriptors and paths
- Process IDs and fork()/exec()
- Traditional page tables and page faults
- POSIX compatibility layer (available as optional shim)

---

## [1.0.0] — Genesis Release

### Added

- Initial architecture specification
- Neural-Symbolic Kernel design
- Semantic Storage Engine design
- Agentic Orchestration Layer design
- Heterogeneous Hardware Abstraction Layer design
- Core system call interface specification
- Context switching with semantic working memory

---

## Roadmap

### [2.1.0] — Q3 2026
- [ ] Distributed CognitaOS across multiple machines
- [ ] Quantum error correction integration
- [ ] Photonic memory (optical RAM) support
- [ ] Advanced agent dreaming (offline learning)

### [2.2.0] — Q4 2026
- [ ] Self-hosting: CognitaOS compiling CognitaOS
- [ ] Formal verification of entire kernel
- [ ] 1M concurrent agents on single node
- [ ] Sub-microsecond context switching

### [3.0.0] — Q2 2027
- [ ] Quantum-native algorithms as first-class citizens
- [ ] Photonic compute crystals
- [ ] Self-aware kernel (meta-cognition)
- [ ] Cross-machine semantic memory federation
