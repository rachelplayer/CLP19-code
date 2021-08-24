/* Some code adapted from Section 5 of https://github.com/shaih/HElib/blob/master/doc/designDocument/he-library.pdf 
 * Some code adapted from SEALExamples
 * This code requires the following changes to be made to HElib:
 *   1) make Ctxt::tensorProduct public so we can do homomorphic multiplication without automatically mod switching or relinearizing
 *   2) (for debugging only) add a function Ctxt::getSize to determine the size of a ciphertext
*/

#include <iostream>
#include <iomanip>

#include "EncryptedArray.h"
#include "FHE.h"
#include "norms.h"

using namespace std;

/*
This function computes, for a given chain of operations, over a user-specified number of trials,
an average observed noise growth in ciphertexts.
*/
void test_noise(int trials);

/* Helper functions */
bool is_power_of_2(long x);
NTL::xdouble get_sum_of_squared_differences(NTL::xdouble mean, vector<NTL::xdouble> array, int size_of_array);
NTL::xdouble get_standard_dev(NTL::xdouble mean, vector<NTL::xdouble> array, int trials);
NTL::xdouble get_noise();
NTL::xdouble get_noise_budget(Ctxt encrypted, FHESecKey secret_key);

bool is_power_of_2(long x) 
{
    return x > 0 && !(x & ( x - 1));
}

NTL::xdouble get_sum_of_squared_differences(NTL::xdouble mean, vector<NTL::xdouble> array, int size_of_array)
{
    NTL::xdouble sum_of_squared_differences(0);
    NTL::xdouble temp(0);

    for (int i=0; i<size_of_array; i++)
    {
        temp = array[i] - mean;
        temp = temp * temp;
        sum_of_squared_differences += temp;
    }
    return sum_of_squared_differences;
}

NTL::xdouble get_standard_dev(NTL::xdouble mean, vector<NTL::xdouble> array, int trials)
{
    NTL::xdouble trials_copy(trials);
    NTL::xdouble denominator = trials_copy - 1;
    NTL::xdouble sum_sq_diff = get_sum_of_squared_differences(mean, array, trials);
    NTL::xdouble variance = sum_sq_diff / denominator;
    NTL::xdouble std_dev = sqrt(variance);
    return std_dev;
}

/* Inspired by the HElib debugging function decryptAndPrint */
NTL::xdouble get_noise(Ctxt encrypted, FHESecKey secret_key)
{
    NTL::ZZX plaintext, noise_poly;
    NTL::ZZ noise_zz;
    NTL::xdouble noise;
    secret_key.Decrypt(plaintext, encrypted, noise_poly);
    noise_zz = largestCoeff(noise_poly);
    conv(noise, noise_zz);
    return noise;
}

NTL::xdouble get_noise_budget(Ctxt encrypted, FHESecKey secret_key)
{
    NTL::xdouble noise, log_noise, log_q, noise_budget;
    noise = get_noise(encrypted, secret_key);
    log_noise = log(noise)/log(NTL::xdouble(2));
    log_q = NTL::xdouble(encrypted.getContext().logOfProduct(encrypted.getPrimeSet())/log(2));
    noise_budget = log_q - log_noise - 1;
    return noise_budget;
}

int main()
{

    while (true)
    {
        cout << "\n HElib noise budget experiments:" << endl << endl;
        cout << "  1. Observed Noise Test" << endl;
        cout << "  0. Exit" << endl;

        int selection = 0;
        cout << endl << "Run example: ";
        if (!(cin >> selection))
        {
            cout << "Invalid option." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        
        switch (selection)
        {
        case 1: {
            int trials;
            cout << "Trials: ";
            if (!(cin >> trials) || (trials < 1))
            {
                cout << "Invalid option." << endl;
                break;
            }
            test_noise(trials);
            break;
        }

        case 0: 
            return 0;

        default:
            cout << "Invalid option."<< endl;
            break;
        }
    }

    return 0;
}

void test_noise(int trials)
{
    NTL::xdouble trials_copy(trials);

    /* Interpret ten trials as debug mode */
    bool verbose = false;
    if (trials == 10)
    {
        verbose = true;
    }

    /* Select parameters appropriate for our experiment */
    //long m = 4096; // polynomial modulus n = 2048
    long m = 8192; // polynomial modulus n = 4096
    //long m = 16384; // polynomial modulus n = 8192
    //long m = 32768; // polynomial modulus n = 16384
    //long p = 3;    // set plaintext modulus t = 3
    long p = 257;
    long r = 1;    // plaintext modulus is actually p^r so we need to fix r = 1
    long s = 1;    // lower bound for number of plaintext slots
    
    /* The function buildModChain creates a moduli chain with nBits worth of ctxt primes. So, we set nBits depending on n, to be the same as the total bit count 
    for the SEAL default parameters. */
    long nBits;
    if (m == 4096)
    {
        nBits = 54; 
    }
    else if (m == 8192)
    {
        nBits = 109; 
    }
    else if (m == 16384)
    {
        nBits = 218;
    }
    else
    {
        nBits = 438;
    }

    /* Set other parameters to HElib defaults */
    long c = 2;    // columns in key switching matrix, default is 2 or 3
    long k = 80;   // security parameter, default is 80 (may not correspond to true bit security)
    
    /* Check that choice of m is ok */
    long check_m = FindM(k, nBits, c, p, r, s, m);
    if (check_m != m)
    {
        cout << "Could not select m = " << m << ". Using m = " << check_m << " instead." << endl;
        m = check_m;
    }

    /* Parameter set corresponding to n = 2048 does not support modulus switching */
    bool is_not_2048 = true;
    if (m == 4096)
    {
        is_not_2048 = false;
    }

    /* Store parameters in context and construct chain of moduli */
    FHEcontext context(m, p, r);
    buildModChain(context, nBits);

    /* Print parameters to screen */
    cout << endl;
    cout << "Encryption parameters: " << endl;
    if (is_power_of_2(m))
    {
        cout << "Polynomial modulus: x^n + 1 for n = " << m / 2 << endl;
    }
    else
    {
        cout << "Using the mth cyclotomic ring for m = " << m << endl;
    }
    IndexSet all_primes(0,context.numPrimes()-1);
    double bit_size_of_q = context.logOfProduct(all_primes)/log(2.0);  // logOfProduct is the natural logarithm
    cout << "Actual total bitsize: " << bit_size_of_q << endl;
    cout << "Requested moduli chain with total bitsize: log q = " << nBits << endl;
    cout << "Plaintext modulus: t = " << p << endl;
    cout << "Noise standard deviation sigma = " << context.stdev << endl;
    cout << endl;

    /* Generate keys */
    FHESecKey secret_key(context);
    const FHEPubKey& public_key = secret_key;
    long secret_key_ID = secret_key.GenSecKey(p);
    addSome1DMatrices(secret_key);
     
    /* Construct plaintext and ciphertext objects */
    EncryptedArray ea(context);
    PlaintextArray plain1(ea);
    PlaintextArray plain2(ea);
    Ctxt encrypted1(public_key);
    Ctxt encrypted2(public_key);
    Ctxt encrypted3(public_key);

    /* Holders for the running total of the observed noises in ciphertexts */
    NTL::xdouble total_fresh_observed(0);
    NTL::xdouble total_add_observed(0);
    NTL::xdouble total_mult_observed(0);
    NTL::xdouble total_mod_switch_observed(0);

    /* Holders for all the observed noises */
    vector<NTL::xdouble> array_fresh_observed;
    array_fresh_observed.reserve(trials);
    vector<NTL::xdouble> array_add_observed;
    array_add_observed.reserve(trials);
    vector<NTL::xdouble> array_mult_observed;
    array_mult_observed.reserve(trials);
    vector<NTL::xdouble> array_mod_switch_observed;
    array_mod_switch_observed.reserve(trials);

    /* Gather noise data over user-specified number of trials */
    for (int i = 0; i < trials; i++)
    {
        /* Encode the values i+1 and i+2 into plaintexts */
        long value1 = i+1;
        long value2 = i+2;
        encode(ea, plain1, value1);
        encode(ea, plain2, value2);

        /* Encrypt the plaintexts into ciphertexts */
        ea.encrypt(encrypted1, public_key, plain1);
        ea.encrypt(encrypted2, public_key, plain2);

        if(i == 0)
        {
            cout << "Bit size of the moduli that the ciphertexts are with respect to: " << endl;
            cout << "Fresh ctxt: bit size of q is " << encrypted1.getContext().logOfProduct(encrypted1.getPrimeSet())/log(2) << endl;
        }

        if(verbose && i == 0)
        {
            NTL::ZZX output1, output2;                          
	        secret_key.Decrypt(output1, encrypted1);
            secret_key.Decrypt(output2, encrypted2);
            cout << "In the first trial we encrypted " << output1[0] << " and " << output2[0] << endl;
        }

        /* What is the noise in the fresh ciphertext encrypted1? */
        auto fresh_noise = get_noise_budget(encrypted1, secret_key);
        total_fresh_observed += fresh_noise;
        array_fresh_observed.push_back(fresh_noise);

        /* Add the ciphertexts in place, overwriting the first argument */
        encrypted1 += encrypted2;

        if(i == 0)
        {
            cout << "Addition: bit size of q is " << encrypted1.getContext().logOfProduct(encrypted1.getPrimeSet())/log(2) << endl;
        }

        /* What is the noise growth when adding two fresh ciphertexts? */
        auto add_noise = get_noise_budget(encrypted1, secret_key);
        total_add_observed += add_noise;
        array_add_observed.push_back(add_noise);

        /*  Multiply the ciphertexts and store the output in encrypted3 */
        encrypted3.tensorProduct(encrypted1, encrypted2);

        if(i == 0)
        {
            cout << "Multiplication: bit size of q is " << encrypted3.getContext().logOfProduct(encrypted3.getPrimeSet())/log(2) << endl;
        }

        if(verbose)
        {
            /* Check that multiplication went as expected */
            if((encrypted3.getSize() == encrypted1.getSize()) || (encrypted3.inCanonicalForm(secret_key_ID)))
            {
                cout << "Something went wrong with multiplication: " << endl;
                cout << "Size of input ciphertext: " << encrypted1.getSize() << endl;
                cout << "Size of output ciphertext: " << encrypted3.getSize() << endl;
                cout << "Output ciphertext in canonical form?: " << encrypted3.inCanonicalForm(secret_key_ID) << endl;
            }
        } 

        /* What is the noise growth when multiplying these two ciphertexts? */
        auto mult_noise = get_noise_budget(encrypted3, secret_key);
        total_mult_observed += mult_noise;
        array_mult_observed.push_back(mult_noise);

        /* Modulus switch the ciphertext encrypted3 */
        if(is_not_2048)
        {
            IndexSet natural_primes = encrypted3.naturalPrimeSet();
            encrypted3.modDownToSet(natural_primes);
        
            if(i == 0)
            {
                cout << "Mod switch: bit size of q is " << encrypted3.getContext().logOfProduct(encrypted3.getPrimeSet())/log(2) << endl;
                cout << endl;
            }
        }

        /* What is the noise growth when modulus switching? */
        auto mod_switch_noise = get_noise_budget(encrypted3, secret_key);
        total_mod_switch_observed += mod_switch_noise;
        array_mod_switch_observed.push_back(mod_switch_noise);
        
    }

    /* Compute the mean of the observed noises */
    auto mean_fresh_observed = total_fresh_observed / trials_copy;
    auto mean_add_observed = total_add_observed / trials_copy;
    auto mean_mult_observed = total_mult_observed / trials_copy;
    auto mean_mod_switch_observed = total_mod_switch_observed / trials_copy;

    /* Compute the standard deviation of the observed noises */
    auto std_dev_fresh = get_standard_dev(mean_fresh_observed, array_fresh_observed, trials);
    auto std_dev_add = get_standard_dev(mean_add_observed, array_add_observed, trials);
    auto std_dev_mult = get_standard_dev(mean_mult_observed, array_mult_observed, trials);
    auto std_dev_mod_switch = get_standard_dev(mean_mod_switch_observed, array_mod_switch_observed, trials);

    /* Print out the results */
    cout << "Fresh ciphertexts c1 and c2:" << endl;
    cout << "Mean noise budget observed: " << mean_fresh_observed  << endl;    
    cout << "Standard deviation of noise budget observed: " << std_dev_fresh << endl;     
    cout << endl;

    cout << "Addition c3 = (c1+c2):" << endl;
    cout << "Mean noise budget observed: " << mean_add_observed << endl;        
    cout << "Standard deviation of noise budget observed: " << std_dev_add << endl;   
    cout << endl; 

    cout << "Multiplication c4 = c3 * c2 = (c1+ c2) * c2: " << endl;  
    cout << "Mean noise budget observed: " << mean_mult_observed << endl;
    cout << "Standard deviation of noise budget observed: " << std_dev_mult << endl;    
    cout << endl;   

    if(is_not_2048)
    {
        cout << "Modulus switching of c4: " << endl; 
        cout << "Mean noise budget observed: " << mean_mod_switch_observed << endl;    
        cout << "Standard deviation of noise budget observed: " << std_dev_mod_switch << endl;    
    cout << endl;
    }

    /* Check that decryption succeeded in the final trial */   
    cout << "Checking that decryption succeeded in the final trial..." << endl;
    PlaintextArray plain3(ea);
    ea.decrypt(encrypted3, secret_key, plain3);
    add(ea, plain1, plain2);
    mul(ea, plain1, plain2);
    if(equals(ea, plain3, plain1))
    {
        cout << "Success!" << endl;
        cout << endl;      

        /* Check if decoding succeeded */
        if(verbose)
        {
            /* Note that plaintext modulus is p = 3, so we expect a wrap-around. 
             *  Choosing a sufficiently large prime, for example p = 241, would avoid this  */
            vector<long> output;
            ea.decrypt(encrypted3, secret_key, output);
            cout << "In the last trial we computed: (" << trials << " + (" << trials << " + 1)) * (" << trials << " + 1)" << " = " << output[0] << " modulo " << p << endl;
        }
    }
    else
    {
        cout << "Something went wrong!" << endl;
        cout << endl;      
    } 
}