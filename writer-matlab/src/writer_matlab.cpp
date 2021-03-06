/*
 * mbsolve: Framework for solving the Maxwell-Bloch/-Lioville equations
 *
 * Copyright (c) 2016, Computational Photonics Group, Technical University of
 * Munich.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 */

#include <mat.h>
#include <writer_matlab.hpp>

namespace mbsolve {

static writer_factory<writer_matlab> factory("matlab");

writer_matlab::writer_matlab() : m_ext("mat")
{
}

writer_matlab::~writer_matlab()
{
}

const std::string&
writer_matlab::get_name() const
{
    return factory.get_name();
}

void
writer_matlab::write(const std::string& file,
                     const std::vector<std::shared_ptr<result> >& results,
                     std::shared_ptr<const device> dev,
                     std::shared_ptr<const scenario> scen) const
{
    MATFile *pmat;
    mxArray *t;

    /* open result file */
    pmat = matOpen(file.c_str(), "w");
    if (pmat == NULL) {
        throw std::invalid_argument("File \"" + file + "\" not found");
    }

    /* put scenario data */
    t = mxCreateDoubleScalar(scen->get_endtime());
    matPutVariable(pmat, "SimEndTime", t);
    mxDestroyArray(t);

    t = mxCreateDoubleScalar(scen->get_timestep_size());
    matPutVariable(pmat, "TimeStepSize", t);
    mxDestroyArray(t);

    t = mxCreateDoubleScalar(scen->get_gridpoint_size());
    matPutVariable(pmat, "GridPointSize", t);
    mxDestroyArray(t);

    /* put device data */
    t = mxCreateDoubleScalar(dev->get_length());
    matPutVariable(pmat, "XDim", t);
    mxDestroyArray(t);

    /* put result data */
    for (auto r : results) {
        /* matlab array is created transposed in order to match order */
        mxArray *var = mxCreateDoubleMatrix(r->get_cols(), r->get_rows(),
                                            mxCOMPLEX);

        auto data_real = r->get_data_real().cbegin();
        auto data_imag = r->get_data_imag().cbegin();

        std::copy(data_real, data_real + r->get_count(), mxGetPr(var));
        std::copy(data_imag, data_imag + r->get_count(), mxGetPi(var));

        matPutVariable(pmat, r->get_name().c_str(), var);

        mxDestroyArray(var);
    }

    /* close result file */
    matClose(pmat);
}

const std::string&
writer_matlab::get_extension() const
{
    return m_ext;
}

}
