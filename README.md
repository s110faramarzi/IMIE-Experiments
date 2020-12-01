# IMIE-Experiments

Sadegh Faramarzi

Implementation of the IMIE control algorithm in calcium fluorescence experiments. The input of maximal instantaneous efficiency (IMIE) is a control algorithm to induce and maintian clusters in a population of synchronized phase oscillators. The theory of the controller is decribed in https://link.springer.com/article/10.1007%2Fs10827-018-0683-y. We have tested the algorithm in a calcium imaging setup at Netoff lab where a set of neurons in rodent brain slice are clustered using IMIE and electrical stimulation through a microelectrode.

closedLoopCluster-comments.c is the program to use the camera and DAQ to record and stimulate brain slices simultaneously. SWIFT algorithm is demonstrated in ieeexplore.ieee.org/document/8026592. 'Xjw_analysis2.m' is used to extract phase of oscillatoins in brain slice using the SWIFT algorithm. To estimate the phase response curve (PRC) prior to each experiment, the 'prc.c' is used to deliver electrical pulses to the brain while the camera is recording.

Email any questions and comments to sfaramar@umn.edu
