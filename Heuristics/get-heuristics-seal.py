#####################################
# This file gives the SEAL results #
# obtained using previous methods   #
# for heuristic estimates.          #
# Parameters and code snippets can  #
# be used to quickly reproduce the  #
# results in sage, after calling    #
# load("heuristics.py")             #
#####################################

# Parameters in our SEAL experiments:
t_binary_encode = 256
t_batch = 65537
log_w = 16
n_2048 = 2048
q_2048 = 18014398492704769
p_2048 = 0 #empty product
n_4096 = 4096
q_4096 = 649037106476272273878613017231361
p_4096 = 36028797005856769
n_8192 = 8192
q_8192 = 421249165509532207033449784325084270503814638792416755348439564289
p_8192 = 23384026155439718179038795187252545630830338768897
n_16384 = 16384
q_16384 = 709803437676948000602048364091891555418250923537528707060865187058871917548681790584120247603231064199191719051874330095291126251521
p_16384 = 39402006011767849740480103175750731143751377801758515458640393481685997778669362535200640978245722829400270131167233

print("SEAL (Table 3): ")
print("n: " + str(n_2048) + ", t: binary")
print(seal_test(n_2048, q_2048, t_binary_encode, p_2048, log_w))
#(27, 26, 0)

print("n: " + str(n_4096) + ", t: binary")
print(seal_test(n_4096, q_4096, t_binary_encode, p_4096, log_w))
#(81, 80, 51, 31)

print("n: " + str(n_8192) + ", t: binary")
print(seal_test(n_8192, q_8192, t_binary_encode, p_8192, log_w))
#(189, 188, 157, 139)

print("n: " + str(n_16384) + ", t: binary")
print(seal_test(n_16384, q_16384, t_binary_encode, p_16384, log_w))
#(408, 407, 375, 358)

print("SEAL (Tables 4 and 8): ")

print("n: " + str(n_2048) + ", t: batch")
print(seal_test(n_2048, q_2048, t_batch, p_2048, log_w))
#(19, 18, 0)

print("n: " + str(n_4096) + ", t: batch")
print(seal_test(n_4096, q_4096, t_batch, p_4096, log_w))
#(71, 70, 32, 23)

print("n: " + str(n_8192) + ", t: batch")
print(seal_test(n_8192, q_8192, t_batch, p_8192, log_w))
#(179, 178, 139, 131)

print("n: " + str(n_16384) + ", t: batch")
print(seal_test(n_16384, q_16384, t_batch, p_16384, log_w))
#(398, 397, 356, 350)

