######################################################################################
# This code is based on code developed for the publication [CS16].                   #
#                                                                                    #
# This code looks for suitable parameters for the BGV and FV schems, given a number  #
# (zeta) of additions and a number (L) of multiplications. It loops through possible #
# values for the chain of primes, computes the size of the resulting ciphertext      #
# noise, and checks whether decryption can be correctly performed. If yes,           #
# output the primes as correct parameters; else increment primes sizes and repeat.   #
#                                                                                    #
# There are a number of changes from the original code:                              #
#   - only consider one relinearization variant, which is the case KSVar = 1         #
#   - remove the schemes YASHE and NTRU from the analysis                            #
#   - only use one plaintext modulus, t = 3                                          #
#   - choose secure parameters according to Homomorphic Encryption Standard          #
#   - rename several parameters for notational consistency:                          #
#      - plaintext modulus p -> t                                                    #
#      - Bclean -> fresh_noise                                                       #
#      - Bscale -> mod_switch_additive_noise                                         #
#      - BKs -> relin_noise                                                          #
#      - phim -> n                                                                   #
#      - T -> w                                                                      #
#      - mm -> log_w                                                                 #
#      - lqLm1 -> bitsize_top_modulus                                                #
#      - LQ -> lst_bitsizes_of_moduli_chain                                          #
#      - lQ -> bitsize_ciphertext_modulus                                            #
#   - rename several parameters for readability:                                     #
#      - fldsz -> field_size                                                         #
#      - cipher_sz -> ciphertext_size                                                #
#      - key_sz -> key_size                                                          #
#      - p_sz -> lst_bitsize_primes                                                  #
#      - fp_sz -> output_lst_bitsize_primes                                          #
#      - stp -> step                                                                 #
#      - mn -> minimum                                                               #
#      - mx -> maximum                                                               #
#      - nn -> current_n                                                             #
#      - csz -> current_ciphertext_size                                              #
#      - ksz -> current_key_size                                                     #
#      - eksz -> current_extended_key_size                                           #
#                                                                                    #
# Notes:                                                                             #
# 1) In FV, we scale the invariant noise by the current modulus q,                   #
#    to avoid working with very small fractions (c.f. heuristics.py)                 #
# 2) This function writes to the output file submitted-version-output.txt            #
# 3) We use the 'critical quantity' noise for BGV and 'invariant noise' for FV       #
#                                                                                    #
# [CS16] Ana Costache and Nigel P. Smart. Which ring based somewhat homomorphic      #
# encryption scheme is best? In Kazue Sako, editor, CT-RSA 2016, volume 9610 of      #
# LNCS, pages 325–340. Springer, Heidelberg, February / March 2016.                  #
######################################################################################

# Initialise parameters for the first experiment
scheme := BGV:
L := 2:
t := 3:
loop := true:

appendto("../../Users/ufai006/noise/experiments/cs15-comparison/submitted-version-output.txt");

while (loop) do
  print(xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx):
  print("scheme" = scheme):
  print("L" = L):
  print("t" = t):

  restart;

  # Set parameters
  sig := 3.2:
  h := 64:
  zeta := 8:    # Number additions
  cm := 1:    # Ring constant is 1 for a power-of-two cyclotomic

  # Tables of parameters from the Homomorphic Encryption Standard (Nov 2018)
  # http://homomorphicencryption.org/wp-content/uploads/2018/11/HomomorphicEncryptionStandardv1.1.pdf
  secure_params_128 := table([2048 = 54, 4096 = 109, 8192 = 218, 16384 = 438, 32768 = 881]):
  secure_params_192 := table([2048 = 37, 4096 = 75, 8192 = 152, 16384 = 305, 32768 = 611]):
  secure_params_256 := table([2048 = 29, 4096 = 58, 8192 = 118, 16384 = 237, 32768 = 476]):

  field_size := n * bitsize_ciphertext_modulus:

  if (scheme = BGV) then
    ciphertext_size := 2 * field_size:
    key_size := 2 * field_size:
    fresh_noise := t * (sqrt(3*n) + 2 * sig * sqrt(n) * (16 * sqrt(h) + 3)):
    mod_switch_additive_noise := t * sqrt(n) * (sqrt(3) + (8 * sqrt(h))/sqrt(3)):
    relin_noise := 8/sqrt(3) * t * ceil(log(Q)/log(w)) * sig * n * w:
    ekey_sz := key_size * (1 + bitsize_ciphertext_modulus/log[2](w)):
    MultFunc := nu1 * nu2:
    DecFunc := p0 / 2:
  elif (scheme = FV) then
    ciphertext_size := 2 * field_size:
    key_size := 2 * field_size:
    m_can_bound := t * sqrt(3*n):
    fresh_noise := t * (m_can_bound + 2 * sig * ( (16 * sqrt(2) / sqrt(3)) * n + 3 * sqrt(n))):
    mod_switch_additive_noise := t * (sqrt(3*n) + (8 * sqrt(2) * n) / sqrt(3)):
    relin_noise := 8/sqrt(3) * t * ceil(log(Q)/log(w)) * sig * n * w:
    ekey_sz := key_size * (1 + bitsize_ciphertext_modulus/log[2](w)):
    MultFunc := (3/q) * nu1 * nu2 + ((1/sqrt(12)) * 2 * t * sqrt(n) * (3 + ((8*sqrt(2)/sqrt(3)) * sqrt(n)) + (40*n/3)) + (2 * m_can_bound)) * nu1 + ((1/sqrt(12)) * 2 * t * sqrt(n) * (3 + ((8*sqrt(2)/sqrt(3)) * sqrt(n)) + (40*n/3)) + (2 * m_can_bound)) * nu2 + (1/sqrt(12)) * 2 * t * sqrt(n) * (3 + ((8*sqrt(2)/sqrt(3)) * sqrt(n)) + (40*n/3)):
    DecFunc := p0 / 2:
  else
    print(Wrong);
  fi:

  lst_bitsize_primes := array(0..L-1):
  output_lst_bitsize_primes := array(0..L-1):
  lst_bitsizes_of_moduli_chain := array(0..L-1):

  finished := 0:
  Best := 10^1000:
  for log_n from 11 to 15 do
    if (finished=0) then
      minimum := 1:
      maximum := 2 + 2048 * fin:
      step := 1:
      log_w := minimum:
      while (log_w < subs(fin=finished, maximum)) do
	      current_n := 2^log_n:
        bitsize_top_modulus := secure_params_128[current_n]: # Ensure bitsize_top_modulus is chosen securely
        lst_bitsizes_of_moduli_chain[L-1] := bitsize_top_modulus:
        current_ciphertext_size := subs({n = current_n, bitsize_ciphertext_modulus=bitsize_top_modulus}, ciphertext_size)/(8.0*1024):
        if (current_ciphertext_size <= Best) then
          # Find the largest prime
          B1 := zeta * fresh_noise:
          if (scheme = BGV) then
            B2 := subs({nu1 = B1, nu2 = B1}, MultFunc):
          else
            B2 := subs({nu1 = B1, nu2 = B1, q = 2^lst_bitsizes_of_moduli_chain[L-1]}, MultFunc):
          fi:
          B2 := subs({w = 2^log_w, Q = 2^lst_bitsizes_of_moduli_chain[L-1], nu = B2}, B2 + relin_noise):
          B2 := subs({nu1 = B1, nu2 = B1}, B2):
          lst_bitsize_primes[L-1] := ceil(evalf(subs({n = current_n, Q = 2^lst_bitsizes_of_moduli_chain[L-1]}, log(B2/mod_switch_additive_noise)/log(2.0)))):
          lst_bitsizes_of_moduli_chain[L-2] := lst_bitsizes_of_moduli_chain[L-1] - lst_bitsize_primes[L-1]:
          ok := true:
          for i from L-2 to 1 by -1 do

            # Find the middle primes
            BT := zeta * 2 * mod_switch_additive_noise:
            if (scheme = BGV) then
              Bd := subs({nu1 = BT, nu2 = BT}, MultFunc):
            else
              Bd := subs({nu1 = BT, nu2 = BT, q = 2^lst_bitsizes_of_moduli_chain[i]}, MultFunc):
            fi:
            Bd := subs({w = 2^log_w, Q = 2^lst_bitsizes_of_moduli_chain[i], nu = Bd}, Bd + relin_noise):
            Bd := subs({nu1 = BT, nu2 = BT}, Bd):
            lst_bitsize_primes[i] := ceil(evalf(subs({n = current_n, Q = 2^lst_bitsizes_of_moduli_chain[i]},log(Bd/mod_switch_additive_noise)/log(2.0)))):
            lst_bitsizes_of_moduli_chain[i-1] := lst_bitsizes_of_moduli_chain[i]- lst_bitsize_primes[i]:
            if (lst_bitsizes_of_moduli_chain[i-1] < 2) then
              ok:=false:
              i:=0:
            fi:   
          od:

          # Now test smallest prime
          lst_bitsize_primes[0] := lst_bitsizes_of_moduli_chain[0]:
          Lhs3 := evalf(subs(p0 = 2^lst_bitsize_primes[0], DecFunc)):
          Rhs3 := evalf(subs({n = current_n}, 2*cm*mod_switch_additive_noise)):
   
          if (ok and Lhs3 > Rhs3) then
            current_ciphertext_size := subs({n = current_n, bitsize_ciphertext_modulus = bitsize_top_modulus}, ciphertext_size)/(8.0*1024):
            current_key_size := subs({n = current_n, bitsize_ciphertext_modulus = bitsize_top_modulus}, key_size)/(8.0*1024):
            for i from 0 to L-1 do
	            output_lst_bitsize_primes[i] := lst_bitsize_primes[i]:
            od:
            current_extended_key_size := evalf(subs({n = current_n, bitsize_ciphertext_modulus = bitsize_top_modulus, w = 2^log_w}, ekey_sz))/(8.0*1024):
            Ans := ([current_n, bitsize_top_modulus, 2^log_w, log_w],[current_ciphertext_size, current_key_size, current_extended_key_size], log(current_ciphertext_size)/log(2.0), output_lst_bitsize_primes):
            finished := 1:
            Best := current_ciphertext_size:
          fi:
        fi:
        log_w := log_w + step:
      od:
    fi:
  od:

  # Print the smallest parameters that support the computation, if any exist
  if (finished = 1) then
    print(Ans):
  else
    print("Unable to find suitable parameters");
  end if;    

  # Generate next params to deal with
  if (scheme = BGV) then
    scheme := FV:
  else
    scheme := BGV:
    if (L = 2) then
        L := 4:
      elif (L = 4) then
        L := 6:
      elif (L = 6) then
        L := 8:
      elif (L = 8) then
        L := 10:
      elif (L = 10) then
        L := 12:
      elif (L = 12) then
        L := 14:
      elif (L = 14) then
        L := 16:
      elif (L = 16) then
        L := 18:
      elif (L = 18) then
        L := 20:
      elif (L = 20) then
        L := 22:
      elif (L = 22) then
        L := 24:
      elif(L = 24) then
        L := 26:
      elif (L = 26) then
        L := 28:
      elif (L = 28) then
        L := 30:
      else
        loop:=false:
    fi:
  fi:
od:

print(Finished):