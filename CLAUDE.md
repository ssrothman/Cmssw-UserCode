# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Quick Start

This is a CMSSW-based particle physics analysis package for EEC (Energy-Energy Correlators) analysis. Key workflows:

**Build and run locally:**
```bash
cmsrel CMSSW_10_6_26
cd CMSSW_10_6_26/src/
cmsenv
git clone git@github.com:ssrothman/Cmssw-UserCode.git SRothman --branch EECs_106x
cd SRothman
git submodule update --recursive --init
# Configure boost as documented in README.md
scram b -j8
cmsRun Analysis/test/test_MC.py
```

**Run with CRAB (high-throughput):**
```bash
cd Analysis/production/crab
python submit.py -y mc_2018UL_DY_HTbin.yaml
```

## Architecture Overview

The codebase is organized into functional modules:

| Module | Purpose |
|--------|---------|
| `Analysis/` | Event selection (Z->mumu), jet building, configuration management |
| `CustomJets/` | SimonJet producer (custom jet constituent collections with systematics baked in) |
| `EECs/` | EEC calculator backends (proj, res3, res4) and NANO table producers |
| `Matching/` | Gen-to-reco track matching with configurable delta-R, pT resolution, charge/flavor filters |
| `DataFormats/` | Custom CMSSW data formats |
| `SimonTools/` | Core particle selection and systematic variation logic |
| `JetToolbox/` | Wrapper for jetToolbox helper functions |

### Data Flow

1. **Input**: MiniAOD events
2. **Event Selection**: Z->mumu selection produces `NANO_selected.root` (pass) and `NANO_dropped.root` (fail)
3. **Jet Building**: AK8 PUPPI jets with preselection flags (OverlapVeto, Preselection)
4. **SimonJet Conversion**: CMSSW jets -> SimonJet collections with systematic variations
5. **Gen Matching**: Tracks matched between gen and reco SimonJets
6. **EEC Calculation**: 3-point and 4-point EECs computed on matched constituents
7. **Output**: NanoAOD with EEC tables, matching info, and systematic variations

### Configuration System

All analysis parameters are in JSON config files (`Analysis/python/config/`):
- `config_basic.json` - Full analysis configuration with event selection, jet cuts, matching parameters, systematic variations, and EEC binning
- Systematics use "nominal + variations" pattern - override only changed settings per variation

### Key Producer Functions

```python
# Event selection
from SRothman.Analysis.setupEventSelections_cff import setupEventSelections

# Jet building
from SRothman.Analysis.setupAK8Jets_cff import setupAK8Jets

# SimonJet production (with systematics)
from SRothman.CustomJets.setupSimonJets import setupSimonJets

# Gen matching
from SRothman.Matching.setupMatching import setupMatching

# EEC calculations
from SRothman.EECs.setupEEC import setupEEC_MC, setupEEC_data
```

## EEC Types

| Type | Description | Dimensionality |
|------|-------------|----------------|
| `proj` | Projected EECs (2-6 point) | 1D (R = delta R) |
| `res3` | Resolved 3-point EECs | 3D (R, r, c) |
| `res4` | Resolved 4-point EECs | 3D subspaces (dipole, tee, triangle) |

Each can output in three formats:
- `Unbinned` - One row per N-tuple (most detail, largest files)
- `Vector` - Prebinned but unmerged
- `Array` - Prebinned with merged bins (most compact)

## Common Development Tasks

### Adding a new systematic variation
Edit `Analysis/python/config/config_basic.json`, add to `Systematics.variations`:
```json
"MY_VARIATION": {
  "EM0scale": "UP",
  "applyPuppi": true
}
```

### Adding a new EEC observable
1. Add calculator in `EECs/src/` (backend)
2. Add producer in `EECs/plugins/`
3. Add table producer in `EECs/plugins/`
4. Register in `EECs/python/common_cff.py`

### Testing
```bash
cmsRun Analysis/test/test_MC.py       # MC test
cmsRun Analysis/test/test_data.py     # Data test
```

## Notes for Claude Code

- Do not modify the Boost library setup - it's a critical build dependency
- All Python config uses CMSSW's EDProducer/EDFilter pattern
- Systematic variations are controlled by "settings" applied to fixed "parameters"
- EEC tables contain `nEntry` field for unflattening constituent arrays
- Gen-matching uses chi-squared likelihood with configurable resolutions per particle type
