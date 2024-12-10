# LETItGo

## Install dependencies

### Fedora 

```sudo yum install gflags-devel graphviz-devel```


# C++ API 

## How to use the library 

It is possible to model LET applications using the library

```
	auto sample = new LETModel();

    TASK_ID t0 = sample->addTask(1, 9, 9);
    TASK_ID t1 = sample->addTask(3, 9, 9);
    TASK_ID t2 = sample->addTask(3, 3, 8);
    TASK_ID t3 = sample->addTask(0, 1, 3);

    sample->addDependency(t0, t2);
    sample->addDependency(t0, t3);
    sample->addDependency(t1, t2);
```

## How to run the experiments

```
./build/src/benchmarkAgelatency -help
    -K_N_only (Limit the search to random periodicity factors K that divide the
      repetition factors N.) type: bool default: false
    -begin_n (Minimal task count) type: int32 default: 5
    -detailed (printout every sample) type: bool default: false
    -end_n (Maximum task count) type: int32 default: 100
    -iter_count (How many run per graph (precision)) type: int32 default: 50
    -kind (Kind of dataset to generate (automotive,generic,harmonic))
      type: string default: "automotive"
    -sample_count (How many graph to generate per size (variety)) type: int32
      default: 100
    -seed (Value of the first seed.) type: int32 default: 123
    -step_n (Step of task count) type: int32 default: 5
    -verbose (Specify the verbosity level (0-10)) type: int32 default: 0
```

./build/src/benchmarkAgelatency -detailed -begin_n 100 -end_n 500 -step_n 100  -sample_count 1  -iter_count 1


## TODOs 

  - Need to discuss data type between integer and floats
