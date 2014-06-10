/* The following are all the constants controlling the behaviour of
   the system and output. */

/* How many samples in the distribution? */
#define NSamples (1000)

/* The prior distribution over samples at the first timestep is a
   Gaussian with the following parameters. */
#define PriorMean (0.0)
//#define PriorSigma (0.2)
#define PriorSigma (10)

/* The process model is a first-order Auto-Regressive Process of the
   following form:

   x_{t+1} - ProcessMean =
     (x_t - ProcessMean) * ProcessScaling + ProcessSigma * w_t

   where w_t is zero-mean unit iid Gaussian noise. */
#define ProcessMean (0)
#define ProcessScaling (1)
#define ProcessSigma (20)

/* The observation density is a mixture of Gaussians, where each
   observed object has a different sigma as follows. */
#define ObsSigma (10)
