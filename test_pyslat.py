#! /mingw64/bin/python3

# This script demonstrates the current state of the pyslat library.

# Import the pyslat library, and other libraries needed to plot 
# the results:
import os
import pyslat
import matplotlib.pyplot as plt
import numpy as np
import math

# Create a non-linear hyperbolic function, as used in example 1, and use it as
# the base function for an IM-Rate relationship:
im = pyslat.factory(pyslat.FUNCTION_TYPE.NLH, [1221, 29.8, 62.2])
im_rate = pyslat.MakeSimpleRelationship(im)

old_slat_path = os.path.expanduser("~") + "/Downloads/SLATv1.15_Public/example1_output_gcc/"
if True:
    im_rate_GCC = np.loadtxt(old_slat_path + "example1_IM-rate-1", skiprows=3, unpack=True)
    old_slat_line, = plt.loglog(im_rate_GCC[0], im_rate_GCC[1])
    old_slat_line.set_label("Old SLAT")
    old_slat_line.set_linewidth(1)

    # Use the numpy and matplotlib libraries to reproduce the IM-Rate
    # diagram from example 1:
    t = np.arange(0.0, 1.001, 0.001)
    s = list(map(im_rate.getlambda, t))
    pyslat_line, = plt.loglog(t, s)
    pyslat_line.set_label("pyslat")
    pyslat_line.set_linewidth(6)
    pyslat_line.set_linestyle(":")
    plt.xlabel('IM (PGA)')
    plt.ylabel('Probability of Exceedence')
    plt.title('IM-Rate Relation')
    plt.grid(True)
    plt.savefig("im-rate.png")
    plt.legend()
    plt.show()


# Create power law functions, and combine them into a lognormal probabilistic
# function, representing the EDP-IM relation from example 1:
edp_mu = pyslat.factory(pyslat.FUNCTION_TYPE.PLC, [0.1, 1.5])
edp_sigma = pyslat.factory(pyslat.FUNCTION_TYPE.PLC, [0.5, 0.0])
edp_im = pyslat.MakeLogNormalProbabilisticFn({pyslat.LOGNORMAL_PARAM_TYPE.MEAN_X:edp_mu,
                                                  pyslat.LOGNORMAL_PARAM_TYPE.SD_LN_X:edp_sigma} )

edp_im_GCC = np.loadtxt(old_slat_path + "example1_EDP-IM-1", skiprows=3, unpack=True)
old_slat_line, = plt.plot(edp_im_GCC[0], edp_im_GCC[1])
old_slat_line.set_label("Old SLAT (mean)")
old_slat_line.set_linewidth(1)

# Use the numpy and matplotlib libraries to reproduce the EDP-IM relationship
# diagram from example 1:
t = np.arange(0.0, 2.51, 0.01)
s50 = list(map(edp_im.X_at_exceedence, t, map(lambda x:0.50, t)))
smean = list(map(edp_im.Mean, t))
s16 = list(map(edp_im.X_at_exceedence, t, map(lambda x:0.16, t)))
s84 = list(map(edp_im.X_at_exceedence, t, map(lambda x:0.84, t)))
plt.plot(t, s16, 'k--', label='16%')
plt.plot(t, s50, 'k:', label='50%')
pyslat_line, = plt.plot(t, smean, 'k:', label='Mean')
pyslat_line.set_linewidth(6)
pyslat_line.set_linestyle(":")
plt.plot(t, s84, 'k', label='84%')
legend=plt.legend(loc='upper center', shadow=True)
legend.get_frame().set_facecolor('#00FFCC')
plt.xlabel('IM (PGA)')
plt.ylabel('EDP (Deck Drift)')
plt.title('EDP-IM Relation')
plt.grid(True)
plt.savefig("edp-im.png")
plt.show()

# Use the functions defined above to describe the relationship between EDP and
# rate:
edp_rate = pyslat.MakeCompoundRelationship(im_rate, edp_im)
edp_rate_GCC = np.loadtxt(old_slat_path + "example1_EDP-rate-1", skiprows=3, unpack=True)
old_slat_line, = plt.loglog(edp_rate_GCC[0], edp_rate_GCC[1])
old_slat_line.set_label("Old SLAT (mean)")
old_slat_line.set_linewidth(1)

# Finally, use the numpy and matplotlib libraries to reproduce the EDP-Rate
# relationship diagram from example 1:
t = np.arange(0.0, 0.15, 0.001)
s = list(map(edp_rate.getlambda, t))
plt.xlabel('EDP (Deck Drift)')
plt.ylabel('Probability of Exceedence')
plt.title('EDP-Rate Relationship')
plt.grid(True)
pyslat_line, = plt.loglog(t, s, label="pyslat")
pyslat_line.set_linewidth(6)
pyslat_line.set_linestyle(":")
plt.xticks([0.001, 0.005, 0.01, 0.05, 0.10, 0.15], ["0.1%", "0.5%", "1%", "5%", "10%", "15%"])
plt.axis([0.0, 0.15, 1E-5, 1E-1])
legend=plt.legend(loc='upper center', shadow=True)
legend.get_frame().set_facecolor('#00FFCC')
plt.savefig("edp-rate.png")
plt.show()


# Make a LogNormalDistribution, as part of developing the class interface:
dist = pyslat.MakeLogNormalDist({pyslat.LOGNORMAL_PARAM_TYPE.MEAN_LN_X: 10.0,
                                 pyslat.LOGNORMAL_PARAM_TYPE.SD_LN_X:1.5})

MU_LN_X = 10.0
SIGMA_LN_X = 1.5
MEDIAN_X = math.exp(MU_LN_X)
MEAN_X = math.exp(MU_LN_X + SIGMA_LN_X * SIGMA_LN_X / 2.0)
SIGMA_X = MEAN_X * math.sqrt(math.exp(SIGMA_LN_X * SIGMA_LN_X) - 1.0)

print("MU_LN_X: ", MU_LN_X, " --> ", dist.get_mu_lnX())
print("SIGMA_LN_X: " , SIGMA_LN_X, " --> ", dist.get_sigma_lnX())
print("MEDIAN_X: ", MEDIAN_X, " --> ", dist.get_median_X())
print("MEAN_X: ", MEAN_X, " --> ", dist.get_mean_X())
print("SIGMA_X: ", SIGMA_X, " --> ", dist.get_sigma_X())


fragility = pyslat.MakeFragilityFn([
    {pyslat.LOGNORMAL_PARAM_TYPE.MEAN_X: 0.0062, pyslat.LOGNORMAL_PARAM_TYPE.SD_LN_X:0.4},
    {pyslat.LOGNORMAL_PARAM_TYPE.MEAN_X: 0.0230, pyslat.LOGNORMAL_PARAM_TYPE.SD_LN_X:0.4},
    {pyslat.LOGNORMAL_PARAM_TYPE.MEAN_X: 0.0440, pyslat.LOGNORMAL_PARAM_TYPE.SD_LN_X:0.4},
    {pyslat.LOGNORMAL_PARAM_TYPE.MEAN_X: 0.0564, pyslat.LOGNORMAL_PARAM_TYPE.SD_LN_X:0.4}])


edp_data = list()
fragility_prob = list()
fragility_high = list()
for i in range(fragility.n_states()):
    fragility_prob.append(list())
    fragility_high.append(list())
    
for i in range(200):
    edp =  i / 1000.
    edp_data.append(edp)
    data = fragility.pExceeded(edp)
    for j in range(len(data)):
        fragility_prob[j].append(data[j])

    data = fragility.pHighest(edp)
    for j in range(len(data)):
        fragility_high[j].append(data[j])

for i in range(fragility.n_states()):
    plt.plot(edp_data, fragility_prob[i], label=i)
plt.xlabel('EDP')
plt.ylabel('Probability')
plt.title('Fragility Functions')
plt.grid(True)
plt.legend()
plt.savefig("fragility-prob.png")
plt.show()

for i in range(fragility.n_states()):
    plt.plot(edp_data, fragility_high[i], label=i)
plt.xlabel('EDP')
plt.ylabel('Probability')
plt.title('Fragility Functions')
plt.grid(True)
plt.legend()
plt.savefig("fragility-high.png")
plt.show()

loss_fn = pyslat.MakeLossFn([
    {pyslat.LOGNORMAL_PARAM_TYPE.MEAN_X: 0.03, pyslat.LOGNORMAL_PARAM_TYPE.SD_LN_X:0.4},
    {pyslat.LOGNORMAL_PARAM_TYPE.MEAN_X: 0.08, pyslat.LOGNORMAL_PARAM_TYPE.SD_LN_X:0.4},
    {pyslat.LOGNORMAL_PARAM_TYPE.MEAN_X: 0.25, pyslat.LOGNORMAL_PARAM_TYPE.SD_LN_X:0.4},
    {pyslat.LOGNORMAL_PARAM_TYPE.MEAN_X: 1.00, pyslat.LOGNORMAL_PARAM_TYPE.SD_LN_X:0.4}])


compgroup = pyslat.MakeCompGroup(edp_rate, fragility, loss_fn, 1);
edp_data = list()
mean_loss_data = list()
sd_ln_loss_data = list()
for i in range(1, 200):
    edp = i / 1000.
    edp_data.append(edp)
    mean_loss_data.append(compgroup.E_Loss_EDP(edp))
    sd_ln_loss_data.append(compgroup.SD_ln_loss_EDP(edp))

mean_loss_GCC = np.loadtxt(old_slat_path + "example1_Loss-EDP-1", skiprows=3, unpack=True)
old_slat_line, = plt.plot(mean_loss_GCC[0], mean_loss_GCC[1])
old_slat_line.set_label("Old SLAT (mean)")
old_slat_line.set_linewidth(1)

pyslat_line, =plt.plot(edp_data, mean_loss_data, label="Mean Loss")
pyslat_line.set_linewidth(6)
pyslat_line.set_linestyle(":")
plt.xlabel('EDP')
plt.ylabel('Mean Loss')
plt.title('Loss vs. EDP')
plt.grid(True)
plt.legend()
plt.savefig("mean-loss-edp.png")
plt.show()

old_slat_line, = plt.plot(mean_loss_GCC[0], mean_loss_GCC[2])
old_slat_line.set_label("Old SLAT (sd)")
old_slat_line.set_linewidth(1)
pyslat_line, = plt.plot(edp_data, sd_ln_loss_data, label="SD ln(loss)")
pyslat_line.set_linewidth(6)
pyslat_line.set_linestyle(":")
plt.xlabel('EDP')
plt.ylabel('SD ln(Loss)')
plt.title('Loss vs. EDP')
plt.grid(True)
plt.legend()
plt.savefig("sd-loss-edp.png")
plt.show()

edp_data = list()
mean_loss_data = list()
sd_ln_loss_data = list()
for i in range(1, 250):
    im = i / 100.
    edp_data.append(im)
    mean_loss_data.append(compgroup.E_Loss_IM(im))
    sd_ln_loss_data.append(compgroup.SD_ln_loss_IM(im))

mean_loss_GCC = np.loadtxt(old_slat_path + "example1_Loss-IM-1", skiprows=3, unpack=True)
old_slat_line, = plt.plot(mean_loss_GCC[0], mean_loss_GCC[1])
old_slat_line.set_label("Old SLAT (mean)")
old_slat_line.set_linewidth(1)

pyslat_line, =plt.plot(edp_data, mean_loss_data, label="Mean Loss")
pyslat_line.set_linewidth(6)
pyslat_line.set_linestyle(":")
plt.xlabel('IM')
plt.ylabel('Mean Loss')
plt.title('Loss vs. IM')
plt.grid(True)
plt.legend()
plt.savefig("mean-loss-im.png")
plt.show()

old_slat_line, = plt.plot(mean_loss_GCC[0], mean_loss_GCC[2])
old_slat_line.set_label("Old SLAT (sd)")
old_slat_line.set_linewidth(1)
pyslat_line, = plt.plot(edp_data, sd_ln_loss_data, label="SD ln(loss)")
pyslat_line.set_linewidth(6)
pyslat_line.set_linestyle(":")
plt.xlabel('IM')
plt.ylabel('SD ln(Loss)')
plt.title('Loss vs. IM')
plt.grid(True)
plt.legend()
plt.savefig("sd-loss-im.png")
plt.show()

print("Expected annual loss: ", compgroup.E_annual_loss())

mean_loss_GCC = np.loadtxt(old_slat_path + "example1_ExpectedLoss-1", skiprows=3, unpack=True)
old_slat_line, = plt.plot(mean_loss_GCC[0], mean_loss_GCC[1])
old_slat_line.set_label("Old SLAT")
old_slat_line.set_linewidth(1)
years = range(1, 100)
expected_loss = list()
expected_loss_per_year = list()
for y in years:
    expected_loss.append(compgroup.E_loss(y, 0.06))
    expected_loss_per_year.append(compgroup.E_loss(y, 0.06)/y)
    
pyslat_line, = plt.plot(years, expected_loss, label="Expected Loss")
pyslat_line.set_linewidth(6)
pyslat_line.set_linestyle(":")
plt.xlabel('Year')
plt.ylabel('Loss')
plt.title('Expected Annual Loss')
plt.grid(True)
plt.legend()
plt.savefig("expected-annual-loss.png")
plt.show()

old_slat_line, = plt.plot(mean_loss_GCC[0], mean_loss_GCC[2])
old_slat_line.set_label("Old SLAT")
old_slat_line.set_linewidth(1)
pyslat_line, = plt.plot(years, expected_loss_per_year, label="Expected Loss Per Year")
pyslat_line.set_linewidth(6)
pyslat_line.set_linestyle(":")
plt.xlabel('Year')
plt.ylabel('Loss')
plt.title('Expected Annual Loss Per Year')
plt.grid(True)
plt.legend()
plt.savefig("expected-annual-loss-per-year.png")
plt.show()



mean_rate_GCC = np.loadtxt(old_slat_path + "example1_Loss-rate-1", skiprows=3, unpack=True)
old_slat_line, = plt.loglog(mean_rate_GCC[0], mean_rate_GCC[1])
old_slat_line.set_label("Old SLAT")
old_slat_line.set_linewidth(1)
rate = list()
for loss in mean_rate_GCC[0]:
    rate.append(compgroup.lambda_loss(loss))
    
pyslat_line, = plt.loglog(mean_rate_GCC[0], rate, label="Loss Rate")
pyslat_line.set_linewidth(6)
pyslat_line.set_linestyle(":")
plt.xlabel('Loss')
plt.ylabel('Rate')
plt.title('Loss Rate')
plt.grid(True)
plt.legend()
plt.savefig("loss-rate.png")
plt.show()
