#! /usr/bin/python3

import pyslat
import matplotlib.pyplot as plt
import numpy as np

im = pyslat.factory(pyslat.FUNCTION_TYPE.NLH, [1221, 29.8, 62.2])
im_rate = pyslat.MakeSimpleRelationship(im)

t = np.arange(0.0, 1.001, 0.001)
s = list(map(im_rate.getlambda, t))
plt.loglog(t, s)
plt.xlabel('IM (PGA)')
plt.ylabel('Probability of Exceedence')
plt.title('IM-Rate Relation')
plt.grid(True)
plt.savefig("im-rate.png")
plt.show()


edp_mu = pyslat.factory(pyslat.FUNCTION_TYPE.PLC, [0.1, 1.5])
edp_sigma = pyslat.factory(pyslat.FUNCTION_TYPE.PLC, [0.5, 0.0])
edp_im = pyslat.MakeLogNormalProbabilisticFunction(edp_mu, edp_sigma)

t = np.arange(0.0, 2.51, 0.01)
s50 = list(map(edp_im.X_at_exceedence, t, map(lambda x:0.50, t)))
s16 = list(map(edp_im.X_at_exceedence, t, map(lambda x:0.16, t)))
s84 = list(map(edp_im.X_at_exceedence, t, map(lambda x:0.84, t)))
plt.plot(t, s16, 'k--', label='16%')
plt.plot(t, s50, 'k:', label='50%')
plt.plot(t, s84, 'k', label='84%')
legend=plt.legend(loc='upper center', shadow=True)
legend.get_frame().set_facecolor('#00FFCC')
plt.xlabel('IM (PGA)')
plt.ylabel('EDP (Deck Drift)')
plt.title('EDP-IM Relation')
plt.grid(True)
plt.savefig("edp-im.png")
plt.show()

edp_rate = pyslat.MakeCompoundRelationship(im_rate, edp_im)

t = np.arange(0.0, 0.15, 0.001)
s = list(map(edp_rate.getlambda, t))
plt.xlabel('EDP (Deck Drift)')
plt.ylabel('Probability of Exceedence')
plt.title('EDP-Rate Relationship')
plt.grid(True)
plt.loglog(t, s)
plt.xticks([0.001, 0.005, 0.01, 0.05, 0.10, 0.15], ["0.1%", "0.5%", "1%", "5%", "10%", "15%"])
plt.axis([0.0, 0.15, 1E-5, 1E-1])
plt.savefig("edp-rate.png")
plt.show()


