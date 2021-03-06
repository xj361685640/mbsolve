import sys
sys.path.append('./mbsolve-lib/')
sys.path.append('./solver-openmp/')
sys.path.append('./writer-matlab/')

import pymbsolvelib as mb
import pysolveropenmp
import pywritermatlab

import numpy as np
import math
import time

# vacuum
mat_vac = mb.material("Vacuum")
mb.material.add_to_library(mat_vac)

# Ziolkowski active region material
qm = mb.qm_desc_2lvl(1e24, 2 * math.pi * 2e14, 6.24e-11, 1.0e10, 1.0e10, -1.0)
mat_ar = mb.material("AR_Ziolkowski", qm)
mb.material.add_to_library(mat_ar)

# Ziolkowski setup
dev = mb.device("Ziolkowski")
dev.add_region(mb.region("Vacuum left", mat_vac, 0, 7.5e-6))
dev.add_region(mb.region("Active region", mat_ar, 7.5e-6, 142.5e-6))
dev.add_region(mb.region("Vacuum right", mat_vac, 142.5e-6, 150e-6))

# scenario
sce = mb.scenario("Basic", 32768, 200e-15)
sce.add_record(mb.record("inv12", 2.5e-15))
sce.add_record(mb.record("e", 2.5e-15))
sce.set_dm_init_type(mb.scenario.lower_full)

# add source
sce.add_source(mb.sech_pulse("sech", 0.0, mb.source.hard_source, 4.2186e9/2,
                             2e14, 10, 2e14))

sol = mb.solver("openmp-2lvl-pc", dev, sce)

print('Solver ' + sol.get_name() + ' started')

tic = time.time()
sol.run()
toc = time.time()

print('Solver ' + sol.get_name() + ' finished in ' + str(toc - tic) + ' sec')

wri = mb.writer("matlab")

outfile = dev.get_name() + "_" + sce.get_name() + "." + wri.get_extension()

results = sol.get_results()

#print("I have " + str(len(results)) + " result(s)")

#d11 = np.matrix(results[0].get_data_complex()).reshape(results[0].get_rows(),
#                                                       results[0].get_cols())

#print(d11)

wri.write(outfile, sol.get_results(), dev, sce)
