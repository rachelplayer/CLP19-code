Evaluating the effectiveness of heuristic worst-case noise analysis in FHE
==========================================================

This repository contains the code used to generate Tables 1--9 in [CLP19].

Experimental data in Tables 1 and 2 was obtained using the provided HElib code [HElib]. Experimental data in Tables 3 and 4 was obtained using the provided Microsoft SEAL code [SEAL]. 

Noise growth estimates using the Iliashenko heuristic bounds [I19]  in Tables 1--4 were obtained using python scripts `heuristics-iliashenko.py`, `get-heuristics-iliashenko-helib.py` and `get-heuristics-iliashenko-seal.py`. Noise growth estimates using previous heuristic bounds [CS16, CLP17] were obtained using python scripts `heuristics.py`, `get-heuristics-helib.py` and `get-heuristics-seal.py`.

Data for the comparison of BGV and FV in Tables 5--9 was obtained by running the provided Maple script `comparison.mpl`, which is based on code developed for a comparison presented in prior work of Costache and Smart [CS16].


Running the code
----------------

**Heuristics** 
The python script are best run using SageMath [SAGE]. To obtain the Iliashenko bounds for HElib (Tables 1 and 2) in Sage run `load("heuristics-iliashenko.py")` then `load("get-heuristics-iliashenko-helib.py")`. To obtain the Iliashenko bounds for SEAL (Tables 3 and 4) run `load("heuristics-iliashenko.py")` then `load("get-heuristics-iliashenko-seal.py")`. To obtain the previous bounds for HElib (Tables 1 and 2) run `load("heuristics.py")` then `load("get-heuristics-helib.py")`. To obtain the previous bounds for SEAL (Tables 3 and 4) run `load("heuristics.py")` then `load("get-heuristics-seal.py")`.

**Comparison**
The file `comparison.mpl` is used to generate Tables 5--9 by selecting the appropriate plaintext modulus (line 50). The file `comparison.mpl` can be run directly provided Maple is installed.

**BGV noise data from HElib**
The HElib files `helib-noise.cpp` and `helib-noise-budget.cpp` were developed to run with the January 2019 [release](https://github.com/shaih/HElib/commits/1.0.0-beta0-Jan2019) of HElib. 

To run any of these files in that version of HElib, follow the instructions [here](https://github.com/shaih/HElib/blob/e1edb4d6088a4103fc5b739b6e89e644627fea9f/INSTALL.txt). 

Note that the files `helib-noise.cpp` and `helib-noise-budget.cpp` all require a slight modification to the Ctxt class, namely that the `Ctxt::tensorProduct()` function is made public.

**FV noise data from SEAL**
The SEAL experiments `examples-invariant.cpp` and  `examples-random-batch.cpp` were developed to for Microsoft SEAL release 3.4, and require the provided `CMakeLists.txt` file to be in `SEAL/native/examples`.

SEAL uses a so-called special prime when `coeff_modulus` (the ciphertext modulus) contains at least two primes. This means that parameter sets that contain only one prime cannot support relinearization or rotation. There are two ways of running the tests: with or without using the special prime. 

(1) To run the experiments with the special prime, build SEAL 3.4.3 as usual, install, and build the noise test [as usual](https://github.com/microsoft/SEAL). That is, in `SEAL/native/examples` run `cmake .` followed by `make`. 

(2) If you want to disable the special prime to see how a plain FV implementation would perform, you must first clone SEAL 3.4.3, then copy `disable_special_prime.patch` to your SEAL directory and type `git apply disable_special_prime.patch`. This will break relinearization and rotation but the full `coeff_modulus` will be used for ciphertexts. After building SEAL, install it, and then rebuild the noise test as described in (1). No change to the noise test code is required.

The SEAL results reported in Tables 3 and 4 were obtained by disabling the special prime. The results in Tables 3 and 4 were obtained using the executable `noisetest`, which corresponds to the file `examples-invariant.cpp`. The executable `noisetest_batched`, which corresponds to the file `examples-random-batch.cpp`, produces data for a batching setting where plaintexts are chosen randomly. The results can be seen to be very similar to those in Table 4.


Bibliography
------------
[CLP17] Hao Chen and Kim Laine and Rachel Player. Simple Encrypted Arithmetic Library - SEAL v2.2. Technical report, Microsoft Research, 2017.

[CLP19] Anamaria Costache and Kim Laine and Rachel Player. Evaluating the effectiveness of heuristic worst-case noise analysis in FHE. Preprint, 2019. Available at https://eprint.iacr.org/2019/493
Accepted for publication at ESORICS 2020.

[CS16] Ana Costache and Nigel P. Smart. Which ring based somewhat homomorphic encryption scheme is best? In Kazue Sako, editor, CT-RSA 2016, volume 9610 of LNCS, pages 325–340. Springer, Heidelberg, February / March 2016.

[HElib] HElib. January 2019. https://github.com/homenc/HElib

[I19] Ilia Iliashenko. Optimisations of fully homomorphic encryption. PhD thesis, KU Leuven, 2019.

[SAGE] https://www.sagemath.org

[SEAL] Microsoft SEAL (release 3.4). Microsoft Research, Redmond, WA. October 2019. https://github.com/microsoft/SEAL