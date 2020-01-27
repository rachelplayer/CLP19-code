#####################################
# This file provides functions for  #
# estimating the noise bounds using # 
# Iliashenko's method for heuristic #
# estimates.                        #
#####################################

########
# SEAL #
########

def seal_q_times_heuristic_noise_fresh(n, t, q):
    r_t_q = q % t 
    sigma = 3.19
    inside = (4./3)*n + 1
    inside *= sigma**2
    inside += r_t_q**2/(12.)
    output_noise = sqrt(inside)
    output_noise *= 6*t*sqrt(n)
    return output_noise

def seal_q_times_heuristic_noise_add(q_times_input_noise_1, q_times_input_noise_2):
    return q_times_input_noise_1 + q_times_input_noise_2

def seal_q_times_heuristic_noise_mult(n, q, t, q_times_input_noise_1, q_times_input_noise_2):
    splodge = t * sqrt(3*n + 2*n**2 + (4/3.)*n**3)
    q_times_v1_factor = t*(sqrt(3*n + 2*n**2))
    q_times_v2_factor = t*(sqrt(3*n + 2*n**2))
    q_times_v1_times_v2 = (1/q)*q_times_input_noise_1*q_times_input_noise_2
    return 3*q_times_v1_times_v2 + splodge + q_times_v1_factor*q_times_input_noise_1 + q_times_v2_factor*q_times_input_noise_2

def seal_q_times_heuristic_noise_relin(n, q, t, log_w, q_times_input_noise):
    ell = floor(log(q)/log_w)
    w = pow(2, log_w)
    sigma = 3.19
    return q_times_input_noise + (t * sqrt(ell + 1) * sqrt(3) * sigma * n * w)

def seal_p_times_heuristic_noise_mod_switch(n, t, q, p, q_times_input_noise):
    scale_factor = q/p
    p_times_input_noise = q_times_input_noise / scale_factor
    return p_times_input_noise + t * (sqrt(3*n) + sqrt(2)*n)

def seal_get_invariant_noise_budget(q_times_noise, q):
    bits_in_q = floor(log(q,2))+1
    q_v_mod_q = math.fmod(q_times_noise,q)
    bits_in_q_v_mod_q = floor(log(q_v_mod_q,2)) + 1
    output = bits_in_q - bits_in_q_v_mod_q - 1
    return max(0, output)

def seal_test(n, q, t, p, log_w):
    fresh = seal_q_times_heuristic_noise_fresh(n, t, q)
    add = seal_q_times_heuristic_noise_add(fresh, fresh)
    mult = seal_q_times_heuristic_noise_mult(n, q, t, add, fresh)
    relin = seal_q_times_heuristic_noise_relin(n, q, t, log_w, mult)
    fresh_budget = seal_get_invariant_noise_budget(fresh, q)
    add_budget = seal_get_invariant_noise_budget(add, q)
    mult_budget = seal_get_invariant_noise_budget(mult, q)
    if(n==2048):
        return fresh_budget, add_budget, mult_budget 
    else:
        mod_switch = seal_p_times_heuristic_noise_mod_switch(n, t, q, p, relin)
        mod_switch_budget = seal_get_invariant_noise_budget(mod_switch, p)
        return fresh_budget, add_budget, mult_budget, mod_switch_budget 

#########
# HElib #
#########

def helib_heuristic_noise_fresh(n, t):
    sigma = 3.19
    inside_sqrt = (4./3)*n + 1
    inside_sqrt *= n*sigma**2
    inside_sqrt += n/(12.)
    output_noise = sqrt(inside_sqrt)
    output_noise *= 6*t
    return output_noise

def helib_heuristic_noise_add(input_noise_1, input_noise_2):
    return input_noise_1 + input_noise_2

def helib_heuristic_noise_mult(input_noise_1, input_noise_2):
    return input_noise_1 * input_noise_2

def helib_heuristic_noise_mod_switch(n, t, q, p, input_noise):
    h = 64
    inside_sqrt = 1 + (2./3)*n
    inside_sqrt *= n/(12.)
    output = sqrt(inside_sqrt)
    output *= 6*t
    output += ((p/q) * input_noise)
    return output

def helib_heuristic_noise_relin(n, q, t, log_w, input_noise):
    ell = floor(log(q)/log_w)
    w = pow(2, log_w)
    sigma = 3.19
    return input_noise + t*sqrt(ell + 1)*n*w*sigma*sqrt(3)

##########################
# Log of estimated noise #
##########################

def log_readable(noise):
    output = log(noise, 2)
    return RR(output)

def helib_test(n, t, q, p):
    fresh = helib_heuristic_noise_fresh(n, t)
    add = helib_heuristic_noise_add(fresh, fresh)
    mult = helib_heuristic_noise_mult(add, fresh)
    out_fresh = log_readable(fresh)
    out_add = log_readable(add)
    out_mult = log_readable(mult)
    if (n > 2048):
        mod_switch = helib_heuristic_noise_mod_switch(n, t, q, p, mult)
        out_mod_switch = log_readable(mod_switch)
        return out_fresh, out_add, out_mult, out_mod_switch
    else:
        return out_fresh, out_add, out_mult

##########################
# Estimate critical      #
# quantity 'noise budget'#
##########################

def helib_get_noise_budget(noise, q):
    bits_in_q = floor(log(q,2))+1
    bits_in_noise = floor(log(noise,2)) + 1
    output = bits_in_q - bits_in_noise - 1
    return output

def helib_noise_budget_test(n, t, q, p):
    fresh = helib_heuristic_noise_fresh(n, t)
    add = helib_heuristic_noise_add(fresh, fresh)
    mult = helib_heuristic_noise_mult(add, fresh)
    fresh_budget = helib_get_noise_budget(fresh, q)
    add_budget = helib_get_noise_budget(add, q)
    mult_budget = helib_get_noise_budget(mult, q)
    if (n > 2048):
        mod_switch = helib_heuristic_noise_mod_switch(n, t, q, p, mult)
        mod_switch_budget = helib_get_noise_budget(mod_switch, p)
        return fresh_budget, add_budget, mult_budget, mod_switch_budget 
    else:
        return fresh_budget, add_budget, mult_budget


