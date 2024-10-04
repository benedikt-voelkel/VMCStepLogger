# MCReplay

This is a pseudo-detector-simulation engine based on the [Virtual Monte Carlo (VMC)](https://vmc-project.github.io/) package. It takes logged steps recorded by the `MCStepLogger` to **replay** the entire simulation.

The main objective is to be able to provide an engine to study the impact of parameter variations with. Eventually, this can be used to optimise (full-)simulation parameters in view of enhancing their speed and efficiency.

The functionality is compiled into a separate library `libMCReplayCore`. Hence, `libMCStepLoggerInterceptSteps` is completely independent which also allows to run the step logging against the `MCReplayEngine` engine.

The replay has been tested and is verified against [GEANT3_VMC](https://github.com/vmc-project/geant3) and [GEANT4_VMC](https://github.com/vmc-project/geant4_vmc) VMC interfaces.
It produces the exact same steps under the dondictions that the geometry does not change and no other (cut) parameters are changed.

The `MCReplayEngine` has been tested with the [ALICE O2 framework](https://github.com/AliceO2Group/AliceO2) and also hits can be reproduced sufficiently which allows for realistic parameter tuning.

## Replay the transport
The `MCReplayEngine` is usually meant to be used inside the original simulation framework. Although this engine can replay the steps, it cannot replay the properties of potentially sensitive volumes so no hits can be produced.
Running a replay can tehrefore be used to study the impact of parameter changes on the physics outcome.

It is important to note that the geometry of a replay run **must** must be identical to the original simulation.

### Set/change production cuts 

The list of tunable parameters is foreseen to correspond to the possible settings implemented for GEANT3_VMC and GEANT4_VMC and can be found [here](include/MCReplay/MCReplayPhysics.h). The parameters can be set via the usual interfaces
* `TVirtualMC::SetCut` and `TVirtualMC::SetProcess` to set global cut and process values,
* `TVirtualMC::Gstpar` to set parameters depending on a certain medium.

**On the other hand**, there is one special parameter called `CUTALLE` which is an energy production cut on **any** PDG. This parameter has no meaning to GEANT3_VMC or GEANT4_VMC.

The application of cuts in GEANT is subtle. The `MCReplayEngine` **approximates** their application to how they are applied in GEANT4_VMC. **NOTE** that there are larger differences before GEANT4_VMC version `v6-5` where the application of cuts has been refined.
It is strongly recommended to use GEANT4_VMC from that version (included) onwards.

**IMPORTANT**:
* Changing any parameters other than `CUTGAM` or `CUTELE` has no effect at this point.
* Using the same `CUTGAM` and `CUTELE` settings in a replay can have minimal impacts on the number of steps. This is due to the complexetiy of their application in GEANT4_VMC.

The exact same steps are done when you set
* `MCReplayEngine::allowStopTrack(false)`: the surrounding simulation framework has no chance to stop a replayed track at any time,
* `MCReplayEngine::blockSetProcessesCuts(true)`: even small differences in how cuts are applied have no impact anymore as absolutely no cuts are applied and steps are really replayed as-is.

### Study impact on hits

There is a lot of RNG going on during a simulation. Between two steps, a transport engine does various calculations which most likely impact the RNG. During a replay, there are no such computations. So even though the same steps are injected into the simulation framework, the RNG states **most likely differ** wrt. the original simulation.
The same RNG might have an impact on the generation of hits (which it does in O2). While steps might be perfectly replayed, hits might not.
To study the impact on hits, it is advised to

1. Run the original simulation as a baseline, produce the `MCStepLogger` file here.
1. Run the `MCReplayEngine` once based on the output while not changing any parameters; this run provides the reference hits.
1. Change parameters and compare the new hits to the reference hits, **not** to the hits from the original run to exclude additional RGN effects.

## Running/testing on step level only (`MCReplayGenericApplication` and `MCReplayGenericStack`)

If only the replay of steps is desired, all required functionality is already included here, completely independent of any environment the reference run was done. Simply have a look at [this code](src/replay.cxx) serving as the source for the executable `mcreplay`. It makes use of the `MCReplayGenericApplication` and `MCReplayGenericStack` just to replay the steps. Once implemented, the impact of the parameters on the stepping can be evaluated with this tool alone. To do so, just run the `MCStepLogger` followed by the replay, for instance as follows (assuming  everything was setup with `aliBuild`)

```bash
MCSTEPLOG_TTREE=1 LD_PRELOAD=$MCSTEPLOGGER_ROOT/lib/libMCStepLoggerIntercept.so mcreplay
```

That will pick up the step file `MCStepLoggerOutput.root`. If you gave that a different name (or in case also your contained tree has another name), you will find the following help message useful:

```bash
> mcreplay --help
Replaying a previously recorded particle transport step-by-step. Mainly meant for checking and performance measurements/optimisation of the transport:
  --help                                show this help message and exit
  --stepfilename arg (=MCStepLoggerOutput.root)
                                        MCStepLogger filename
  --steptreename arg (=StepLoggerTree)  treename inside file where to find step
                                        tree
  --geofilename arg (=o2sim_geometry.root)
                                        ROOT geometry filename
  --geokeyname arg                      key name inside geo file where to find
                                        geometry tree
  -n [ --nevents ] arg (=-1)            number of events to replay
  -e [ --energycut ] arg (=-1)          energy cut to be applied [GeV]
```

As you can see the geometry is required by this executable as well since it has otherwise no idea what the geometry should look like from the `MCStepLoggerOutput.root` file alone.

