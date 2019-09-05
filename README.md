Homomorphic noise growth in practice: comparing BGV and FV
==========================================================

This repository contains the code used to generate Tables 1--7 and Table 9 in the paper [CLP19], available here as 2019-493.pdf

Experimental data in Tables 1, 2 and 5 was obtained using the provided HElib code [HElib]. Experimental data in Tables 3, 4, 6 and 7 was obtained using the provided Microsoft SEAL code [SEAL]. Heuristic bounds in these Tables were obtained using the provided python script `heuristics.py`.

Data for the comparison of BGV and FV in Table 9 was obtained by running the provided Maple script `comparison.mpl`, which is based on code developed for a comparison presented in prior work of Costache and Smart `[CS16]`.

Running the code
----------------

The files `heuristics.py` and `comparison.mpl` can be run directly provided Python and Maple respectively are installed.

**HElib**
The HElib files `helib-noise.cpp`, `helib-noise-budget.cpp` and `helib-invariant-noise-budget.cpp` were developed to run with the January 2019 [release](https://github.com/shaih/HElib/commits/1.0.0-beta0-Jan2019) of HElib. 

To run any of these files in that version of HElib, follow the instructions [here](https://github.com/shaih/HElib/blob/e1edb4d6088a4103fc5b739b6e89e644627fea9f/INSTALL.txt). 

Note that the files `helib-noise.cpp`, `helib-noise-budget.cpp` and `helib-invariant-noise-budget.cpp` all require a slight modification to the Ctxt class, namely that the `Ctxt::tensorProduct()` function is made public.

**SEAL**
The SEAL files were developed to run with [Microsoft SEAL](https://github.com/microsoft/SEAL/) release 3.1.
The commits corresponding to specific releases are not tagged, we used commit aa7bf57.

The easiest way to run the files with that version of SEAL:
- replace the file examples/examples.cpp with either `examples-invariant.cpp` or `examples-scaled-inherent.cpp`
- rename this file as `examples.cpp`
- follow the instructions on how to build the examples [here](https://github.com/microsoft/SEAL/blob/aa7bf57aa11a91d9ca8712816550ae68793add99/README.md)

Note that `examples-scaled-inherent.cpp` requires modifications to the `Decryptor` class that are provided as separate files.

Bibliography
------------

[CS16] Ana Costache and Nigel P. Smart. Which ring based somewhat homomorphic encryption scheme is best? In Kazue Sako, editor, CT-RSA 2016, volume 9610 of LNCS, pages 325–340. Springer, Heidelberg, February / March 2016.

[CLP19] Anamaria Costache and Kim Laine and Rachel Player. Homomorphic noise growth in practice: comparing BGV and FV. Preprint, 2019. Available at https://eprint.iacr.org/2019/493

[HElib] HElib. January 2019. https://github.com/homenc/HElib

[SEAL] Microsoft SEAL (release 3.1). Microsoft Research, Redmond, WA. December 2018. https://github.com/microsoft/SEAL
