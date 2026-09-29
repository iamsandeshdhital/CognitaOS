# CognitaOS Security Model

## Overview

Security in CognitaOS is not a permission check. Security is a **formal proof** that a computation is safe, verified before the computation is committed.

## The Three Pillars

| Pillar | Mechanism | Guarantee |
|--------|-----------|-----------|
| **Proof Obligations** | SMT solvers + theorem provers | Every operation is formally verified |
| **Capability Web** | Delegation graph with formal edges | No privilege escalation possible |
| **Quantum-Native Crypto** | QKD + CRYSTALS-Kyber/Dilithium | Information-theoretic + post-quantum security |

## Proof Obligations

Every neural decision is accompanied by a proof obligation:

```
Decision: "Switch to agent A, allocate 4 NPU slices, pin 256 pages"

Proof obligation: "Given current system state S, this decision does not violate:
  - I_mem: No memory safety violation
  - I_energy: Total energy within budget
  - I_deadline: All deadlines can be met
  - I_security: No capability escalation
  - I_causal: No causal ordering violation"
```

### Verification Pipeline

```
Neural Decision
    |
    v
[1] Generate proof obligation (formal statement)
    |
    v
[2] SMT solver (Z3) -- fast constraint check
    |         |
    |         v (if unknown)
    |    [3] Theorem prover (Lean 4) -- deep verification
    |         |
    v         v
[4] Decision: ACCEPT / REJECT / FALLBACK
```

### Invariant Definitions

```c
/* Memory safety: no out-of-bounds access */
#define I_mem "forall p in pages: p in allocated_pages(entity)"

/* Energy budget: total energy within allocation */
#define I_energy "sum(energy(e) for e in entities) < energy_budget"

/* Deadline: all deadlines can be met */
#define I_deadline "forall e in entities: predicted_completion(e) < deadline(e)"

/* Security: no capability escalation */
#define I_security "forall c in capabilities: c in granted_caps(entity)"

/* Causal: no causal ordering violation */
#define I_causal "forall (a, b) in dependencies: happens_before(a, b)"
```

## The Capability Web

Capabilities are nodes in a directed graph where edges represent delegation:

```
[User: intent.submit]
       |
       v
[Agent: researcher]
       |
       +---> [cap: store.read]
       +---> [cap: pdf.parse]
       +---> [cap: llm.infer]
       +---> [cap: memory.write]

[Agent: communicator]
       |
       +---> [cap: email.send]
       +---> [cap: template.render]
```

### Delegation Rules

1. **No delegation to untrusted agents** — An agent cannot delegate capabilities to an agent with a lower sandbox level
2. **No capability amplification** — Delegation cannot create new capabilities
3. **No transitive escalation** — If A delegates to B and B delegates to C, C cannot have more capabilities than A
4. **Revocable** — Any delegation can be revoked by the delegator

### Formal Verification

Every delegation edge is verified by the symbolic reasoner:

```
Edge: Agent A delegates cap X to Agent B

Proof: cap X in granted_caps(A) AND
       sandbox_level(B) <= sandbox_level(A) AND
       NOT (cap X in forbidden_caps(B))
```

## Quantum-Native Security

### Quantum Key Distribution

All inter-crystal communication uses QKD over the photonic interconnect:

```c
struct qkd_channel {
    uint32_t    qc_key_rate_bps;        /* 1 Mbps */
    uint32_t    qc_error_rate;          /* QBER */
    uint8_t     qc_shared_key[4096];
    uint64_t    qc_key_age_ns;
};
```

### Post-Quantum Cryptography

All persistent data is encrypted with:
- **CRYSTALS-Kyber** — Key encapsulation mechanism
- **CRYSTALS-Dilithium** — Digital signatures

### Zero-Trust Architecture

- **No implicit trust** — Every operation is verified
- **No perimeter** — Security is end-to-end
- **No persistent sessions** — Keys are ephemeral, refreshed every 100ms

## Threat Model

| Threat | Mitigation |
|--------|-----------|
| Memory corruption | Formal verification of all memory access |
| Privilege escalation | Capability web with formal delegation proofs |
| Side-channel attacks | Photonic interconnect with QKD |
| Quantum computing attacks | CRYSTALS-Kyber/Dilithium for all persistent data |
| Kernel exploitation | Self-modifying kernel with formal verification |
| Agent compromise | Sandboxing, capability isolation, audit trail |
| Replay attacks | Ephemeral keys, Merkle-chained audit receipts |

## Audit Trail

Every operation produces a cryptographic receipt:

```c
struct cog_audit_receipt {
    uint64_t    receipt_id;
    uint64_t    intent_id;
    uint64_t    timestamp_ns;
    uint8_t     prev_hash[32];      /* Merkle chain */
    uint8_t     payload_hash[32];
    uint8_t     signature[64];      /* Ed25519 by kernel */
};
```

The audit trail is an append-only Merkle tree, making tampering detectable.
