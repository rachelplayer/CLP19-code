#include <iomanip>
#include <vector>
#include <string>
#include <chrono>
#include <random>
#include <thread>
#include <mutex>
#include <memory>
#include <limits>
#include <complex>
#include <iterator>

#include "seal/seal.h"

using namespace std;
using namespace seal;

/*
Helper function: Prints the name of the example in a fancy banner.
*/
void print_example_banner(string title)
{
    if (!title.empty())
    {
        size_t title_length = title.length();
        size_t banner_length = title_length + 2 + 2 * 10;
        string banner_top(banner_length, '*');
        string banner_middle = string(10, '*') + " " + title + " " + string(10, '*');

        cout << endl
            << banner_top << endl
            << banner_middle << endl
            << banner_top << endl
            << endl;
    }
}

/*
Helper function: Prints the parameters in a SEALContext.
*/
void print_parameters(const shared_ptr<SEALContext> &context)
{
    auto &context_data = *context->key_context_data();

    /*
    Which scheme are we using?
    */
    string scheme_name;
    switch (context_data.parms().scheme())
    {
    case scheme_type::BFV:
        scheme_name = "BFV";
        break;
    case scheme_type::CKKS:
        scheme_name = "CKKS";
        break;
    default:
        throw invalid_argument("unsupported scheme");
    }

    cout << "/ Encryption parameters:" << endl;
    cout << "| scheme: " << scheme_name << endl;
    cout << "| poly_modulus_degree: " << 
        context_data.parms().poly_modulus_degree() << endl;

    /*
    Print the size of the true (product) coefficient modulus.
    */
    cout << "| coeff_modulus size: " << 
        context_data.total_coeff_modulus_bit_count() << " bits" << endl;

    /*
    For the BFV scheme print the plain_modulus parameter.
    */
    if (context_data.parms().scheme() == scheme_type::BFV)
    {
        cout << "| plain_modulus: " << context_data.
            parms().plain_modulus().value() << endl;
    }

    cout << "\\ noise_standard_deviation: " <<
        util::global_variables::noise_standard_deviation << endl;
    cout << endl;
}

/*
This function computes, for a given chain of operations, over a user-specified number of trials,
- the average observed noise growth in ciphertexts,  
- the standard deviation of the noise growth in ciphertexts
*/
void test_noise(int trials);

int main()
{
#ifdef SEAL_VERSION
    cout << "SEAL version: " << SEAL_VERSION << endl;
#endif
    while (true)
    {
        cout << "\nModified SEAL Examples:" << endl << endl;
        cout << "  1. Invariant Noise Test" << endl;
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

/* Helper functions for computing standard deviation. */
double get_sum_of_squared_differences(double mean, vector<double> array, int size_of_array)
{
    double sum_of_squared_differences = 0;
    double temp = 0;
    for (int i=0; i<size_of_array; i++)
    {
        temp = array[i] - mean;
        temp = temp * temp;
        sum_of_squared_differences += temp;
    }
    return sum_of_squared_differences;
}


double get_standard_dev(double mean, vector<double> array, int trials)
{
    double denominator = trials - 1;
    double sum_sq_diff = get_sum_of_squared_differences(mean, array, trials);
    double variance = sum_sq_diff / denominator;
    double std_dev = sqrt(variance);
    return std_dev;
}

void test_noise(int trials)
{
    /* 
        If n = 2048, q is too small for mod switching.
    */
    bool is_n_2048 = false;

    /* Set the parameters in a SEALContext */
    EncryptionParameters parms(scheme_type::BFV);
    if(is_n_2048)
    {
        parms.set_poly_modulus_degree(2048);
        parms.set_coeff_modulus(CoeffModulus::BFVDefault(2048));
    }
    else
    {    
        //parms.set_poly_modulus_degree(4096);
        //parms.set_coeff_modulus(CoeffModulus::BFVDefault(4096));
        parms.set_poly_modulus_degree(8192);
        parms.set_coeff_modulus(CoeffModulus::BFVDefault(8192));
        //parms.set_poly_modulus_degree(16384);
        //parms.set_coeff_modulus(CoeffModulus::BFVDefault(16384));
    }    
    
    /* Choose a typical plaintext modulus t for IntegerEncoder */
    //parms.set_plain_modulus(1 << 8);

    /* Need to choose t such that t is a prime number congruent to 1 modulo 2n */
    parms.set_plain_modulus(65537);

    auto context = SEALContext::Create(parms);
    print_parameters(context);

    /* Construct an encoder */
    //IntegerEncoder encoder(context); // binary encoder
    BatchEncoder encoder(context);

    /* Only needed for batching */
    auto qualifiers = context->first_context_data()->qualifiers();
    cout << "Batching enabled: " << boolalpha << qualifiers.using_batching << endl;
    bool using_keyswitching = context->using_keyswitching();
    cout << "Keyswitching enabled: " << boolalpha << using_keyswitching << endl;
    size_t slot_count = encoder.slot_count();
    size_t row_size = slot_count / 2;
    auto print_matrix = [row_size](auto &matrix)
    {
        cout << endl;

        /*
        We're not going to print every column of the matrix (there are 2048). Instead
        print this many slots from beginning and end of the matrix.
       */
        size_t print_size = 5;

        cout << "    [";
        for (size_t i = 0; i < print_size; i++)
        {
            cout << setw(3) << matrix[i] << ",";
        }
        cout << setw(3) << " ...,";
        for (size_t i = row_size - print_size; i < row_size; i++)
        {
            cout << setw(3) << matrix[i] << ((i != row_size - 1) ? "," : " ]\n");
        }
        cout << "    [";
        for (size_t i = row_size; i < row_size + print_size; i++)
        {
            cout << setw(3) << matrix[i] << ",";
        }
        cout << setw(3) << " ...,";
        for (size_t i = 2 * row_size - print_size; i < 2 * row_size; i++)
        {
            cout << setw(3) << matrix[i] << ((i != 2 * row_size - 1) ? "," : " ]\n");
       }
        cout << endl;
    };

    /* Construct other object needed for the experiment */
    KeyGenerator keygen(context);
    PublicKey public_key = keygen.public_key();
    SecretKey secret_key = keygen.secret_key();
    RelinKeys relin_keys = using_keyswitching ? keygen.relin_keys() : RelinKeys();
    Encryptor encryptor(context, public_key);
    Evaluator evaluator(context);
    Decryptor decryptor(context, secret_key);
    Ciphertext encrypted1, encrypted2;
    Plaintext plain1, plain2;

    /* Holders for the running total of the observed noise budgets in ciphertexts */
    double total_fresh_observed = 0;
    double total_add_observed = 0;
    double total_mult_observed = 0;
    double total_relin_observed = 0;
    double total_mod_switch_observed = 0;

    /* Holders for all the observed noise budgets */
    vector<double> array_fresh_observed;
    array_fresh_observed.reserve(trials);
    vector<double> array_add_observed;
    array_add_observed.reserve(trials);
    vector<double> array_mult_observed;
    array_mult_observed.reserve(trials);
    vector<double> array_relin_observed;
    if (using_keyswitching)
    {
        array_relin_observed.reserve(trials);
    }
    vector<double> array_mod_switch_observed;
    if(!is_n_2048)
    {
        array_mod_switch_observed.reserve(trials);
    }

    /* Gather noise data over user-specified number of trials */
    for (int i = 2; i < trials+2; i++)
    {
        /* Encode the raw data into plaintexts */
        //plain1 = encoder.encode(i);
        //plain2 = encoder.encode(i-1);
        vector<uint64_t> pod_matrix1;
        vector<uint64_t> pod_matrix2;
        for (size_t j = 0; j < slot_count; j++)
        {
            pod_matrix1.push_back(j);
            pod_matrix2.push_back(j+1);
        }      
        encoder.encode(pod_matrix1, plain1);
        encoder.encode(pod_matrix2, plain2);

        /* Generate fresh ciphertexts */
        encryptor.encrypt(plain1, encrypted1);
        encryptor.encrypt(plain2, encrypted2);

        /* What is the noise in the fresh ciphertext encrypted1? */
        auto fresh_noise_budget = decryptor.invariant_noise_budget(encrypted1);
        total_fresh_observed += fresh_noise_budget;
        array_fresh_observed.push_back(fresh_noise_budget);

        /* Add the ciphertexts in place, overwriting the first argument */
        evaluator.add_inplace(encrypted1, encrypted2);

        /* What is the noise growth when adding two fresh ciphertexts? */
        auto add_noise_budget = decryptor.invariant_noise_budget(encrypted1);
        total_add_observed += add_noise_budget;
        array_add_observed.push_back(add_noise_budget);

        /*  Multiply the ciphertexts in place, overwriting the first argument. */
        evaluator.multiply_inplace(encrypted1, encrypted2);

        /* What is the noise growth when multiplying these two ciphertexts? */
        auto mult_noise_budget = decryptor.invariant_noise_budget(encrypted1);
        total_mult_observed += mult_noise_budget;
        array_mult_observed.push_back(mult_noise_budget);

        /* Relinearize the ciphertext encrypted1 in place */ 
        if (using_keyswitching)
        {
            evaluator.relinearize_inplace(encrypted1, relin_keys);

            /* What is the noise growth when relinearizing? */
            auto relin_noise_budget = decryptor.invariant_noise_budget(encrypted1);
            total_relin_observed += relin_noise_budget;
            array_relin_observed.push_back(relin_noise_budget);
        }

        if(!is_n_2048)
        {
            /* Modulus switch to the next modulus in the chain */
            evaluator.mod_switch_to_next_inplace(encrypted1);
        
            /* What is the noise growth when modulus switching? */
            auto mod_switch_noise_budget = decryptor.invariant_noise_budget(encrypted1);
            total_mod_switch_observed += mod_switch_noise_budget;
            array_mod_switch_observed.push_back(mod_switch_noise_budget);
        }
    }

    /* Compute the mean of the observed noises */
    auto mean_fresh_observed = total_fresh_observed/trials;
    auto mean_add_observed = total_add_observed/trials;
    auto mean_mult_observed = total_mult_observed/trials;
    auto mean_relin_observed = using_keyswitching ? total_relin_observed/trials : 0.0;
    double mean_mod_switch_observed;
    if(!is_n_2048)
    {
        mean_mod_switch_observed = total_mod_switch_observed/trials;
    }

    /* Compute the standard deviation of the observed noises */
    double std_dev_fresh = get_standard_dev(mean_fresh_observed, array_fresh_observed, trials);
    double std_dev_add = get_standard_dev(mean_add_observed, array_add_observed, trials);
    double std_dev_mult = get_standard_dev(mean_mult_observed, array_mult_observed, trials);
    double std_dev_relin = using_keyswitching ? (mean_relin_observed, array_relin_observed, trials) : 0.0;
    double std_dev_mod_switch;
    if(!is_n_2048)
    {
        std_dev_mod_switch = get_standard_dev(mean_mod_switch_observed, array_mod_switch_observed, trials);
    }

    /* Print out the results */
    cout << "Fresh ciphertexts c1 and c2:" << endl;
    cout << "Mean noise budget observed: "  << mean_fresh_observed << " bits" << endl; 
    cout << "Standard deviation of noise budget observed: " << std_dev_fresh << endl;    
    cout << endl; 

    cout << "Addition c3 = (c1+c2):" << endl;
    cout << "Mean noise budget observed: " << mean_add_observed << " bits" << endl;
    cout << "Standard deviation of noise budget observed: " << std_dev_add << endl;        
    cout << endl; 

    cout << "Multiplication c4 = c3 * c2 = (c1+ c2) * c2: " << endl;
    cout << "Mean noise budget observed: "  << mean_mult_observed << " bits" << endl;
    cout << "Standard deviation of noise budget observed: " << std_dev_mult << endl;        
    cout << endl;   

    if (using_keyswitching)
    {
        cout << "Relinearization of c4: " << endl; 
        cout << "Mean noise budget observed: " << mean_relin_observed << " bits" << endl;
        cout << "Standard deviation of noise budget observed: " << std_dev_relin << endl;  
        cout << endl;      
    }

    if(!is_n_2048)
    {
        cout << "Mod switching of c5: " << endl; 
        cout << "Mean noise budget observed: " << mean_mod_switch_observed << " bits" << endl;
        cout << "Standard deviation of noise budget observed: " << std_dev_mod_switch << endl; 
        cout << endl;     
    }

    /* Check that decryption succeeded in the final trial */
    cout << "Checking that decryption succeeded..." << endl;
    Plaintext plain_result;
    decryptor.decrypt(encrypted1, plain_result);

    /* Decode the resulting plaintext */
    //cout << "In the final trial we compute (" << trials+1 << " plus " << trials << "), multiplied by " << trials << endl;
    //cout << "Plaintext polynomial: " << plain_result.to_string() << endl;
    //cout << "Decoded integer: " << encoder.decode_int32(plain_result) << endl;
    cout << "In the n^th slot, we compute (n-1 + n) * n: " << endl;
    vector<uint64_t> pod_result;
    encoder.decode(plain_result, pod_result);
    cout << "Decoded matrix: " << endl;
    print_matrix(pod_result);
}


