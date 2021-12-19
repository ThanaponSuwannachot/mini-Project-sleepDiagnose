#ifndef MD_MODEL-NORMAL_H
#define MD_MODEL-NORMAL_H

const unsigned int model_mu_dim1 = 3;

const float model_mu[3] = {
    0.007046512684615385, 0.013648587507692307, 0.002951542973076922
};

const unsigned int model_inv_cov_dim1 = 3;
const unsigned int model_inv_cov_dim2 = 3;

const float model_inv_cov[3][3] = {
    278337.66110181215, -22727.852095408543, -441112.0503077989, 
    -22727.852095408547, 79568.3889892913, -90557.58133398659, 
    -441112.05030779896, -90557.58133398659, 1600360.9798940928
};

#endif //MD_MODEL-NORMAL_H