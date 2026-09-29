/**
 * @file hal.h
 * @brief CognitaOS Hardware Abstraction Layer Internal API
 */

#ifndef COGNITA_HAL_H
#define COGNITA_HAL_H

#include <cognita/os.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================
 * Compute Crystal
 * ======================================================================== */

struct compute_crystal {
    cog_hw_kind_t    cc_kind;
    uint64_t        cc_units;
    uint64_t        cc_allocated_units;
    float       cc_utilization;
    float       cc_power_watts;
    float       cc_temperature_c;
};

int hal_crystal_discover(struct compute_crystal **crystals,
                         uint32_t *count);
int hal_crystal_allocate(cog_hw_kind_t kind, uint64_t units,
                         struct compute_crystal **crystal);
int hal_crystal_submit(struct compute_crystal *crystal, const char *kernel,
                       const void *args, size_t args_len,
                       uint64_t *work_item_id);
int hal_crystal_sync(struct compute_crystal *crystal, uint64_t work_item_id,
                     uint64_t timeout_ms, void **result, size_t *result_len);

/* ========================================================================
 * Photonic Interconnect
 * ======================================================================== */

struct photonic_interconnect {
    uint32_t    pi_channel_count;
    uint32_t    pi_channels_per_fiber;
    float       pi_bandwidth_per_channel;
};

int hal_photonic_init(struct photonic_interconnect **pi);
int hal_photonic_send(struct photonic_interconnect *pi, uint32_t channel,
                      const void *data, size_t len);
int hal_photonic_recv(struct photonic_interconnect *pi, uint32_t channel,
                      void *data, size_t len);

/* ========================================================================
 * Quantum Coprocessor
 * ======================================================================== */

struct qpu_coprocessor {
    uint32_t    qc_qubit_count;
    float       qc_t1_coherence_us;
    float       qc_t2_coherence_us;
    float       qc_gate_fidelity_1q;
    float       qc_gate_fidelity_2q;
};

int hal_qpu_discover(struct qpu_coprocessor **qpu);
int hal_qpu_execute(struct qpu_coprocessor *qpu, const void *circuit,
                    size_t circuit_len, uint64_t shots, void **result);

/* ========================================================================
 * Zero-Copy Memory Fabric
 * ======================================================================== */

int hal_zerocopy_init(void **fabric);
int hal_zerocopy_register(void *fabric, void *addr, size_t len,
                          uint64_t *handle);
int hal_zerocopy_transfer(void *fabric, uint64_t src_handle,
                          uint64_t dst_handle, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* COGNITA_HAL_H */
