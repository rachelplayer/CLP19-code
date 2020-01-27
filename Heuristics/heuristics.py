########################
#    SEAL Estimates    #
########################

def seal_q_times_heuristic_noise_fresh(n, t, q):
    r_t_q = q % t 
    sigma = 3.19
    output_noise = t * sqrt(3*n) * r_t_q
    second_term = t * 2 * sigma * (3*sqrt(n) + (16*sqrt(2)/sqrt(3))*n)
    output_noise += second_term
    return output_noise

def seal_q_times_heuristic_noise_add(q_times_input_noise_1, q_times_input_noise_2):
    return q_times_input_noise_1 + q_times_input_noise_2

def seal_q_times_heuristic_noise_mult(n, q, t, q_times_input_noise_1, q_times_input_noise_2):
    splodge = (1/sqrt(12)) * 2 * t * sqrt(n) *(3 + ((8*sqrt(2)/sqrt(3))*sqrt(n)) + (40*n/3))
    q_times_v1_factor = splodge + (2 * t * sqrt(3*n))
    q_times_v2_factor = splodge + (2 * t * sqrt(3*n))
    q_times_v1_times_v2 = (1/q)*q_times_input_noise_1*q_times_input_noise_2
    return 3*q_times_v1_times_v2 + splodge + q_times_v1_factor*q_times_input_noise_1 + q_times_v2_factor*q_times_input_noise_2

def seal_p_times_heuristic_noise_mod_switch(n, t, q, p, q_times_input_noise):
    scale_factor = q/p
    p_times_input_noise = q_times_input_noise / scale_factor
    return p_times_input_noise + t * (sqrt(3*n) + ((8*sqrt(2)/sqrt(3))*n))

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
    fresh_budget = seal_get_invariant_noise_budget(fresh, q)
    add_budget = seal_get_invariant_noise_budget(add, q)
    mult_budget = seal_get_invariant_noise_budget(mult, q)
    if(n==2048):
        return fresh_budget, add_budget, mult_budget
    else:
        mod_switch = seal_p_times_heuristic_noise_mod_switch(n, t, q, p, mult)
        mod_switch_budget = seal_get_invariant_noise_budget(mod_switch, p)
        return fresh_budget, add_budget, mult_budget, mod_switch_budget

#########
# HElib #
#########

def helib_old_heuristic_noise_fresh(n, t):
    sigma = 3.19
    first = n*sigma*32*sqrt(2/3.) 
    second = 6.*sigma*sqrt(n)
    third = sqrt(3.*n)
    output_noise = first + second + third
    output_noise *= t
    return output_noise

def helib_heuristic_noise_add(input_noise_1, input_noise_2):
    return input_noise_1 + input_noise_2

def helib_heuristic_noise_mult(input_noise_1, input_noise_2):
    return input_noise_1 * input_noise_2

def helib_old_heuristic_noise_mod_switch(n, t, q, p, input_noise):
    output = t*sqrt(3.*n) + 16*t*n*sqrt(1/18.)
    output += ((p/q) * input_noise)
    return output

##########################
# Log of estimated noise #
##########################

def log_readable(noise):
    output = log(noise, 2)
    return RR(output)

def helib_test(n, t, q, p):
    fresh = helib_old_heuristic_noise_fresh(n, t)
    add = helib_heuristic_noise_add(fresh, fresh)
    mult = helib_heuristic_noise_mult(add, fresh)
    out_fresh = log_readable(fresh)
    out_add = log_readable(add)
    out_mult = log_readable(mult)
    if (n > 2048):
        mod_switch = helib_old_heuristic_noise_mod_switch(n, t, q, p, mult)
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
    fresh = helib_old_heuristic_noise_fresh(n, t)
    add = helib_heuristic_noise_add(fresh, fresh)
    mult = helib_heuristic_noise_mult(add, fresh)
    fresh_budget = helib_get_noise_budget(fresh, q)
    add_budget = helib_get_noise_budget(add, q)
    mult_budget = helib_get_noise_budget(mult, q)
    if (n > 2048):
        mod_switch = helib_old_heuristic_noise_mod_switch(n, t, q, p, mult)
        mod_switch_budget = helib_get_noise_budget(mod_switch, p)
        return fresh_budget, add_budget, mult_budget, mod_switch_budget 
    else:
        return fresh_budget, add_budget, mult_budget

##########################
# Mentioned in Section 6 #
# and Appendix I:        #
# FV scaled inherent     #
# noise heuristic bounds #
##########################

def seal_heuristic_new_noise(n, t, q):
    sigma = 3.19
    output_noise = 3*sqrt(n)
    second_term = (16*sqrt(2)/sqrt(3))*n
    output_noise += second_term
    return output_noise*(t/q)*2*sigma

def seal_heuristic_noise_add_new_noise(input_noise_1, input_noise_2, t, q, n):
    r_t_q = q % t
    output_noise = (t/q) * 3 * sqrt(3*n) * r_t_q
    output_noise += input_noise_1 
    output_noise += input_noise_2
    return output_noise

def seal_heuristic_noise_mult_new_noise(n, q, t, input_noise_1, input_noise_2):
    Delta = floor(q/t)
    r_t_q = q % t
    output_noise = t*sqrt(3*n)*r_t_q/q + 4*r_t_q*n*t**2/(3*q) #r_t_q*t*a_0/q
    output_noise += r_t_q*Delta*sqrt(3*n)*t**2/(q**2) #-r_t_q*delta*t*[m_1m_2]_t/q**2
    output_noise += r_t_q*Delta*sqrt(3*n)*t**2/(q**2) + 4*n*r_t_q*Delta*t**3/(3*q**2)  #t**2r_t_q*Delta*a_0/q**2
    output_noise += Delta*sqrt(3*n)*t**2*(input_noise_1 + input_noise_2)/q #t*delta*(m_2v_1 + m_1v_2)/q
    output_noise += (r_t_q*t*sqrt(3*n))*(2*t*sqrt(n)/sqrt(3) * (3 + 8*sqrt(2)*sqrt(n)/sqrt(3) + 40*n/3) + 2*Delta*sqrt(3*n)*t**2/q + input_noise_1 + input_noise_2)/q #-r_t_q(m_2a_1t + m_1a_2t)
    output_noise += input_noise_1*input_noise_2 #self-explanatory
    output_noise += 2*input_noise_1*input_noise_2 + (input_noise_1 + input_noise_2)*((2*t*sqrt(n)/sqrt(12))*(3 + 8*sqrt(2*n)/sqrt(3) + 40*n/3) + Delta*sqrt(3*n)*t**2/q) #v_2a_1t + v_1a_2t
    output_noise += 2*t*sqrt(n)/(q*sqrt(12))*(3 + 8*sqrt(2*n)/sqrt(3) + 40*n/3) #(t/q)*sum_i epsilon_i*s^i
    return output_noise

def seal_heuristic_noise_relin_new_noise(n, q, t, log_w, input_noise):
    ell = floor(log(q)/log_w)
    w = pow(2, log_w)
    sigma = 3.19
    return input_noise + (t * (ell + 1) * (8/sqrt(3)) * sigma * n * w)/q

def seal_heuristic_noise_mod_switch_new_noise(n, t, q, p, input_noise):
    r_t_q = q%t
    r_t_p = p%t
    output_noise = t*sqrt(3*n)*(r_t_p/p - r_t_q/q)
    output_noise += (t/p) * ((sqrt(3*n) + ((8*sqrt(2)/sqrt(3))*n)))
    return output_noise + input_noise
