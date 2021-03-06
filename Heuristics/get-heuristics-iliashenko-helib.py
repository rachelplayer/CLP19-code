#####################################
# This file gives the HElib results #
# obtained using Iliashenko's       #
# method for heuristic estimates.   #
# Parameters and code snippets can  #
# be used to quickly reproduce the  #
# results in sage, after calling    #
# load("heuristics-iliashenko.py")  #
#####################################

t = 3
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


print("HElib noise (Table 1):")

print("n: " + str(n_2048))
print(helib_test(n_2048,t, q_2048, p_2048))
#(17.5721332966680, 18.5721332966680, 36.1442665933360)

print("n: " + str(n_4096))
print(helib_test(n_4096, t, q_4096, p_4096))
#(18.5659755402598, 19.5659755402598, 38.1319510805196, 14.1123099915160)

print("n: " + str(n_8192))
print(helib_test(n_8192, t, q_8192, p_8192))
#(19.5616054312303, 20.5616054312303, 40.1232108624605, 15.1043536375399)

print("n: " + str(n_16384))
print(helib_test(n_16384, t, q_16384, p_16384))
#(20.5585072883623, 21.5585072883623, 42.1170145767247, 16.0987010444677)


print("HElib noise budget (Table 2):")

print("n: " + str(n_2048))
print(helib_noise_budget_test(n_2048, t, q_2048, p_2048))
#(35, 34, 16)

print("n: " + str(n_4096))
print(helib_noise_budget_test(n_4096, t, q_4096, p_4096))
#(89, 88, 69, 39)

print("n: " + str(n_8192))
print(helib_noise_budget_test(n_8192, t, q_8192, p_8192))
#(197, 196, 176, 147)

print("n: " + str(n_16384))
print(helib_noise_budget_test(n_16384, t, q_16384, p_16384))
#(416, 415, 394, 366)
