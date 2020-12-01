
//parts from DAQ 
#include <NIDAQmx.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h> 
#include <complex.h>
#include <string.h>
#include <sys/time.h> 



// -------------------------------------
#include "edtinc.h"
#ifdef DO_JPEG
#include "edt_jpeg.h"
#endif

#define FOURGB ((double) (0x1000000) * 16.0)

/*
 * defines
 */
#define STARTSIG 0xa5
#define ENDSIG   0x5a

///////////////////////////////////////////////////////////////////////// This part is from the DAQ code
#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

//#define PI	3.1415926535
int32 CVICALLBACK DoneCallback(TaskHandle taskHandle, int32 status, void *callbackData);


////////////////////////////////////////////////////////////////////////


{

       
/////////////////////////////////////////////// copied from DAQ code
	int			error=0;
	TaskHandle	taskHandle=0;
	char		errBuff[2048]={'\0'};
	


	/*********************************************/
	// DAQmx Configure Code
	/*********************************************/
	DAQmxErrChk (DAQmxCreateTask("",&taskHandle));
	DAQmxErrChk (DAQmxCreateAOVoltageChan(taskHandle,"Dev1/ao0","",-10.0,10.0,DAQmx_Val_Volts,""));

	/*********************************************/
	// DAQmx Start Code
	/*********************************************/
	DAQmxErrChk (DAQmxStartTask(taskHandle));

	/*********************************************/
	// DAQmx Write Code
	/*********************************************/


////////////////////////////////////////////////////////////////////////////////

    int     i, lc;
    int     waited = 0;
    int     unit = 0;
    char   *unitstr = "0";
    int     loops = 1;
    int     wait_for_enter = 0;
    int     user_delay = 0;
    int     numbufs = 4;
    int     do_ring = 1;
    int     verbose = 0;
    int     do_file = 0;
    int     rbtimeout = 0;
    int     bytesread;
    int     started;
    int     done;
    char    cfgfname[128];
    char    bmpfname[128];
    //char    straa[128];
    char    straaa[128];
    char    jpgfname[128];
    char    rawfname[128];
    char    edt_devname[256];
    char   *progname = "take";
    u_char *imagebuf = NULL;
    uint16_t *imagebuf16 = NULL;
    u_char *subimage = NULL;
    u_char *image_p = NULL;
    PdvDev *pdv_p;
    int     timeouts=0, last_timeouts=0;
    int     in_timeout_cleanup = 0;
    int     overruns=0, overrun;
    char    tmpname[128];
    int     subsample = 0, crop = 0;
    int     debug_timeout = 0;
    int     width, height, depth, imagesize, dmasize, bufsize, db;
    int     s_width, s_height, s_depth, s_size, s_db;
    int     crop_x0, crop_x1, crop_y0, crop_y1;
    int     exposure, gain, offset;
    int     set_exposure = 0, set_gain = 0, set_offset = 0;
    int     set_timeout = 0;
    int     do_timestamp = 0;
    int     do_timetype = 0;
    int     timetype = 0;
    int     do_lasttest = 0;
    int     timeout_val = 0;
    int     enable_burst_en = 1;
    int     si = -1;
    int     sc = -1;
    int     vskip, vactv, hskip, hactv;
    int     set_roi = 0;
    int     unset_roi = 0;
    int     checksig = 0, checkcount = 0;
    int     checksim = 0;
    int     checkclsim = 0;
    int     checkclk = 0;
    int     badstart = 0, badend = 0;
    int     just_print_the_last_one = 0;
    u_short startsig = STARTSIG, endsig = ENDSIG;
    u_int   sigcount;
    int     siginit = 0;
    char    errstr[64];
    u_int   timestamp[2];
    u_int   freqbuf[2];
    double  dtime;
    double  svtime = 0;
    double  curtime = 0;
    double  frequency = 0;
    double  sumsize = 0;
    int     checkfrm = 0;
    int     binning = 0;
    int     opto_trigger = 0;
    int     fldid_trigger = 0;

    int     channel = 0;


    int     mosaic = 0;
    int     mosaic_sample = 0;
    int     sample_height = 10;
    int     mosaic_size = 0;
    int     mosaic_width = 0;
    int     mosaic_height = 0;
    char    dvcmode[8];
    unsigned char *mosaic_ptr = NULL;
    unsigned char *mosaic_image = NULL;
    int     arg_channel = -1;
    int     dl;

    int noreset = 0;
    //u_char ** DataArray;
    float weightArr[65536];
    FILE *weightFile=fopen("wm.txt","r");
    int px;
    
    for (px = 0; px < 65536;px++)
    {
       //fscanf(weightFile, "%8f\n", &weightArr[px]);  
       weightArr[px]=1.0;   //when there is no weight array
       //printf("%8f\n", weightArr[px]);
       
    }
    fclose(weightFile);

   double clmt[65536];  //an array for pre-defined clusters
   double pixels[65536];
   int pxl;
    //struct threeNum num;
   FILE *fptr;
   if ((fptr = fopen("wm.bin","rb")) == NULL){
       printf("Error! opening file");
       // Program exits if the file pointer returns NULL.
       exit(1);
   }
     fread(pixels,sizeof(pixels),1,fptr); 
   for(pxl = 0; pxl < 65536; ++pxl)
   {
       clmt[pxl]=pixels[pxl];
       //printf("%f ", clmt[pxl]);
   }
   fclose(fptr); 
    /// SWIFT parameters
    double pi = 3.141593;
    //int my_array[1999];
////////////////////////////////////////
//////////// Specify parameters here
////////////////////////////////////////
    double CF = 5.0;  //center frequency
    int SR = 100;    //Fs
    double w; 
    double complex z1 = 0.0+1.0 * I;  //defining a complex number
    double tau; 
    //float my_array[1999] = {0};
    #define nf 4500 //number of frames 

    double timecomp[nf][5];
    double timecompdiff[nf][5];
    int difframe;
    
    double  stim[nf]={0.0};
    int aa;
    double phase[nf][5];
   
    char fileName[] = "prc021220-10";

////////////////////////////////////////
//////////////////// stim
////////////////////////////////////////

    //double  srand[nf]={0.0};
    //printf("%d\n",(int)(nf/13));

// generating sparse random numbers or stim signal
    double  srand[((int)(nf/100))]={0.0};

    int bd;
    int ad=0;
    for (aa=0;aa<((int)(nf/10));aa++)
    {
    //stim[aa]=rand() % 5 -2;  //random stim
    //////////////////// //regular stim
    //if (aa%100==0)   
    //{ 
    srand[aa]=(rand() % 10 +100); 
    bd=(int)(srand[aa]);
    ad=ad+bd;
    //printf("%d\n",ad);
    if (ad < nf){
     stim[ad]=0.5;
     //printf(" %d\n %f\n",ad,stim[ad]);

    }

    
   // printf("%d\n",stim[ad]);
   // }
    //////////////////// 
    //printf("%f\n",stim[aa]);
    }

    stim[0]=0.0; //later is used for returning hte stim to background value


    //int N=sizeof(my_array)/sizeof(my_array[0]);
    double complex Xjw[nf][4]={0.0+0.0*I};
    w= 2*pi*CF/SR;
    tau = 4*SR/CF;
    double complex D = cexp(-1/tau)*cexp(z1*w);
   ///////////////////////////////    


#ifdef NO_MAIN
    char  **argv = 0;
    int     argc = 0;

    opt_create_argv("take", command_line, &argc, &argv);
#endif


    cfgfname[0] = '\0';
    rawfname[0] = '\0';
    bmpfname[0] = '\0';
    jpgfname[0] = '\0';
    edt_devname[0] = '\0';
    dvcmode[0] = '\0';
    tmpname[0] = '\0';
    ASnum = 0;
    quiet = 0 ;
    siginit = 0 ;

#ifdef __APPLE__
    /* debug for interrupted wakeup */
    signal(SIGINT,gotit);
#endif

    --argc;
    ++argv;
    while (argc && ((argv[0][0] == '-') || (argv[0][0] == '/')))
    {
        switch (argv[0][1])
        {
            case 'u':		/* device unit number */
                ++argv;
                --argc;
                if (argc < 1) 
                {
                    usage(progname, "Error: option 'u' requires a numeric argument\n", 1);
                }
                if ((argv[0][0] >= '0') && (argv[0][0] <= '9'))
                {
                    unitstr = argv[0];
                }
                else 
                {
                    usage(progname, "Error: option 'u' requires a numeric argument\n", 1);
                }
                break;

            case 'B':		/* turn off burst enable */
                ++argv;
                --argc;
                if (argc > 0)
                    enable_burst_en = atoi(argv[0]);
                else
                    enable_burst_en = 1;
                break;

            case 'v':		/* verbose */
                verbose = 1;
                break;

            case 'w':		/* wait for user <cr> between frames */
                wait_for_enter = 1;
                break;

            case 'T':		/* on timeout, dump data to timeout.raw */
                debug_timeout = 1;
                break;

            case 'd':		/* timestamp */
                do_timestamp = 1;
                break;

            case 'D':		/* dvc mode setting (diagnostic) */
                ++argv;
                --argc;
                strcpy(dvcmode, argv[0]);
                break;

            case 'X':		/* use timer */
                ++argv;
                --argc;
                timetype = atoi(argv[0]);
                do_timetype = 1;
                break;

            case 'y':
                opto_trigger = 1;
                break;

            case 'Y':
                fldid_trigger = 1;
                break;

            case 'J':
                just_print_the_last_one = 1;
                break;

            case 'L':
                do_lasttest = 1;	/* freerun/bytecount with wait for next frame */
                break;

            case 'm':		/* microsec sleep between frames */
                ++argv;
                --argc;
                if (argc < 1) 
                {
                    usage(progname, "Error: option 'm' requires a numeric argument\n", 1);
                }
                if ((argv[0][0] >= '0') && (argv[0][0] <= '9'))
                {
                    user_delay = atoi(argv[0]);
                }
                else 
                {
                    usage(progname, "Error: option 'm' requires a numeric argument\n", 1);
                }
                break;

            case 'N':		/* specify numbufs */
                ++argv;
                --argc;
                if (argc < 1) 
                {
                    usage(progname, "Error: option 'N' requires a numeric argument\n", 1);
                }
                if ((argv[0][0] >= '0') && (argv[0][0] <= '9'))
                {
                    numbufs = atoi(argv[0]);
                    if (!numbufs)
                        do_ring = 0; /* if 0 just use pdv_read not ring buffers */
                }
                else 
                {
                    usage(progname, "Error: option 'N' requires a numeric argument\n", 1);
                }
                break;

            case 'H':		/* check count signature from testgen */
                checksim = 1;
                checksig = 0;
                checkcount = 0;
                break;

            case 'P':		/* check count signature from PCI CD sim */
                checkcount = 1;
                checksig = 0;
                break;

            case 'p':		/* check fixed signature from PCI CD sim */
                checksig = 1;
                checkcount = 0;
                break;

            case 'G':		/* check count signature from testgen */
                checkclsim = 1;
                checksim = 0;
                checksig = 0;
                checkcount = 0;
                break;

            case 'q':		/* binning */
                ++argv;
                --argc;
                binning = atoi(argv[0]);
                break;

            case 'Q':		/* turn on quiet mode */
                quiet = 1;
                break;

            case 't':		/* microsec sleep between frames */
                /* if no subarg, set to -1 (auto timeout) */
                if ((argc < 2) || (argv[1][0] == '-'))
                {
                    set_timeout = 1;
                    timeout_val = -1;
                }
                else
                {
                    ++argv;
                    --argc;
                    set_timeout = 1;
                    if ((argv[0][0] >= '0') && (argv[0][0] <= '9'))
                        timeout_val = atoi(argv[0]);
                }
                break;

            case 'l':		/* loops */
                ++argv;
                --argc;
                if (argc < 1) 
                {
                    usage(progname, "Error: option 'l' requires a numeric argument\n", 1);
                }
                if ((argv[0][0] >= '0') && (argv[0][0] <= '9'))
                {
                    loops = atoi(argv[0]);
                }
                else 
                {
                    usage(progname, "Error: option 'l' requires a numeric argument\n", 1);
                }
                break;

            case 'f':		/* save filename */
                ++argv;
                --argc;
                strcpy(rawfname, argv[0]);
                do_file = 1;
                break;

            case 'S':		/* subsample */
                ++argv;
                --argc;
                subsample = atoi(argv[0]);
                break;

            case 'C':		/* crop x0, y0, x1, y1 */
                crop = 1;
                ++argv;
                --argc;
                crop_x0 = atoi(argv[0]);

                ++argv;
                --argc;
                crop_y0 = atoi(argv[0]);

                ++argv;
                --argc;
                crop_x1 = atoi(argv[0]);

                ++argv;
                --argc;
                crop_y1 = atoi(argv[0]);
                break;

            case 'n':		/* autosequence start only */
                ++argv;
                --argc;
                ASnum = atoi(argv[0]);
                break;

            case 's':		/* autosequence start and inc */
                ++argv;
                --argc;
                ASnum = atoi(argv[0]);
                break;

            case 'e':		/* exposure time */
                ++argv;
                --argc;
                set_exposure = 1;
                exposure = atoi(argv[0]);
                break;

            case 'g':		/* gain */
                ++argv;
                --argc;
                set_gain = 1;
                gain = atoi(argv[0]);
                break;

            case 'o':		/* offset (blacklevel) */
                ++argv;
                --argc;
                set_offset = 1;
                offset = atoi(argv[0]);
                break;

            case 'I':		/* # to increment for multiple file writes */
                ++argv;
                --argc;
                ASinc = atoi(argv[0]);
                break;

#ifdef DO_JPEG
            case 'j':
                ++argv;
                --argc;
                strcpy(jpgfname, argv[0]);
                break;
#endif

            case 'b':		/* bitmap save filename */
            case 'i':
                ++argv;
                --argc;
                strcpy(bmpfname, argv[0]);
                do_file = 1;
                break;

            case 'c':		/* channel */
                ++argv;
                --argc;
                if (argc < 1) 
                {
                    usage(progname, "Error: option 'c' requires a numeric argument\n", 1);
                }
                if ((argv[0][0] >= '0') && (argv[0][0] <= '9'))
                {
                    arg_channel = atoi(argv[0]);
                }
                else 
                {
                    usage(progname, "Error: option 'c' requires a numeric argument\n", 1);
                }
                break;

            case 'r':		/* ROI */
                if (argc < 5)
                {
                    usage(progname, "Error: option 'r' requires subsequent arguments", 1);
                    exit(1);
                }
                ++argv;
                --argc;
                hskip = atoi(argv[0]);
                ++argv;
                --argc;
                hactv = atoi(argv[0]);
                ++argv;
                --argc;
                vskip = atoi(argv[0]);
                ++argv;
                --argc;
                vactv = atoi(argv[0]);
                set_roi = 1;
                break;

            case 'R':		/* disable ROI */
                unset_roi = 1;
                break;

            case 'O':		/* try noreset - will be obsolete on clink? */
                noreset = 1 ;
                break;

            case 'x':		/* experimental -- strobe testing */
                ++argv;
                --argc;
                si = atoi(argv[0]);
                ++argv;
                --argc;
                sc = atoi(argv[0]);
                break;

            case 'M':		/* mosaic */
                mosaic = 1;
                break;

            case 'a':		/* call pdv_checkfrm */
                checkfrm = 1;
                break;
            case 'A':
                checkclk = 1;
                break;

            default:
                fprintf(stderr, "unknown flag -'%c'\n", argv[0][1]);
            case '?':
            case 'h':
                usage(progname, "", 0);
                exit(0);
        }
        argc--;
        argv++;
    }

	
    unit = edt_parse_unit_channel(unitstr, edt_devname, "pdv", &channel);

    if (arg_channel != -1)
    {
        channel = arg_channel;
    }

    unit = edt_parse_unit(unitstr, edt_devname, "pdv");

    /*
     * pdv_open_channel is just pdv_open with an extra argument, the channel,
     * which is normally 0 unless you're running multiple cameras (2nd base
     * mode on a PCI DV C-Link or dasy-chained RCI boxes off a single PCI
     * FOI)
     */
    if ((pdv_p = pdv_open_channel(edt_devname, unit, channel)) == NULL)
    {
        sprintf(errstr, "pdv_open(%s%d_%d)", edt_devname, unit, channel);
        pdv_perror(errstr);
        return (1);
    }

    if (pdv_is_simulator(pdv_p))
    {
        fprintf(stderr, "%s unit %d is a simulator board; take is only for use with framegrbbers\n", edt_devname, unit);
        pdv_close(pdv_p);
        exit(1);
    }

    pdv_flush_fifo(pdv_p);

    if (*dvcmode)
        pdv_set_mode_dvc(pdv_p, dvcmode);

    if (set_exposure)
        pdv_set_exposure(pdv_p, exposure);

    if (set_gain)
        pdv_set_gain(pdv_p, gain);

    if (set_offset)
        pdv_set_blacklevel(pdv_p, offset);

    if (set_timeout)
        pdv_set_timeout(pdv_p, timeout_val);

    if (opto_trigger)
        pdv_enable_external_trigger(pdv_p, PDV_PHOTO_TRIGGER);

    else if (fldid_trigger)
        pdv_enable_external_trigger(pdv_p, PDV_FLDID_TRIGGER);


    if (set_roi)
    {
        pdv_set_roi(pdv_p, hskip, hactv, vskip, vactv);
        pdv_enable_roi(pdv_p, 1);
    }
    else if (unset_roi)
    {
        pdv_enable_roi(pdv_p, 0);
    }

    if (binning)
    {
        pdv_set_binning(pdv_p, binning, binning);
    }

    if (checksig)
        take_printf("checking PCI CD Simulator fixed signature -- use simple_putframe -p\n");
    if (checksim)
        take_printf("checking testgen signature\n");
    if (checkclsim)
        take_printf("checking cl simulator signature\n");
    if (checkcount)
        take_printf("checking PCI CD Simulator count signature -- use simple_putframe -P\n");

    if (do_timetype)
    {
        edt_set_timetype(pdv_p, EDT_TM_FREQ);
        edt_get_reftime(pdv_p, freqbuf);
        frequency = (double) freqbuf[0] * FOURGB + freqbuf[1];
        edt_set_timetype(pdv_p, timetype);
    }

    /*
     * set size variables for image and initial subimage
     */
    width = s_width = pdv_get_width(pdv_p);
    height = s_height = pdv_get_height(pdv_p);
    depth = s_depth = pdv_get_depth(pdv_p);
    db = s_db = bits2bytes(depth);
    imagesize = s_size = pdv_get_imagesize(pdv_p);
    dmasize = s_size = pdv_get_dmasize(pdv_p);

    subimage = NULL;
    /*
     * turn off merge
     */
    edt_set_merge(pdv_p, 0, 0, 0, 0);

    if (s_size <= 0)
    {
        printf("%s%d: Invalid image size. Make sure device has been initialized.\n", edt_devname, unit);
        exit(1);
    }

    if (mosaic)
    {
        mosaic_sample = width * sample_height * db;

        mosaic_size = loops * mosaic_sample;

        mosaic_image = pdv_alloc(mosaic_size);

        mosaic_ptr = mosaic_image;

        mosaic_width = width * db;

        mosaic_height = loops * sample_height;

    }

    if (do_ring)
    {
        /* Construct a ring buffer */
        if (pdv_multibuf(pdv_p, numbufs) == -1)
        {
            char    msg[64];

            sprintf(msg, "pdv_multibuf(0x%p, %d)", pdv_p, numbufs);
            pdv_perror(msg);
            return (0);
        }
    }
    else
    {
        bufsize = ((dmasize > imagesize)? dmasize: imagesize);
        imagebuf = (u_char *) pdv_alloc(bufsize);
       
        if (imagebuf == NULL)
        {
            pdv_perror("data buf malloc");
            exit(1);
        }
    }

    pdv_cl_reset_fv_counter(pdv_p) ;

    /*
     * check for crazy subsample and crop values and sort out the params as
     * needed
     */

    if (do_file && (subsample || crop))
    {
        if ((subsample < 0)
                || (subsample > height / 4)
                || (subsample > width / 4))
        {
            printf("-S <subsample> arg out of range\n");
            usage(progname, "", 1);
            exit(1);
        }

        /* force subsample since its used as an arg in processing */
        if (crop && (subsample == 0))
            subsample = 1;

        if (crop)
        {
            int     tmp;

            if (crop_x0 > crop_x1)
            {
                tmp = crop_x1;
                crop_x1 = crop_x0;
                crop_x0 = tmp;
            }
            if (crop_y0 > crop_y1)
            {
                tmp = crop_y1;
                crop_y1 = crop_y0;
                crop_y0 = tmp;
            }

            if (crop_x0 < 0)
                crop_x0 = 0;
            if (crop_y0 < 0)
                crop_y0 = 0;
            if (crop_x1 > width - 1)
                crop_x1 = width - 1;
            if (crop_y1 > height - 1)
                crop_y1 = height - 1;
        }
        else
        {
            crop_x0 = 0;
            crop_y0 = 0;
            crop_x1 = width - 1;
            crop_y1 = height - 1;
        }

        s_width = ((crop_x1 - crop_x0) + subsample) / subsample;
        s_height = ((crop_y1 - crop_y0) + subsample) / subsample;
        s_size = s_width * s_height * s_db;
        subimage = (u_char *) malloc(s_size);
    }

    /*
     * should normally only be set from the config file as "start_delay:",
     * but for testing allow it to be set here too
     */
    if (user_delay)
        pdv_p->dd_p->start_delay = user_delay;

    if (pdv_p->dd_p->start_delay)
        take_printf("%d millisecond delay added between images\n", pdv_p->dd_p->start_delay);
    if (pdv_p->dd_p->force_single)
        take_printf("force_single set; prestarting one image at a time\n");

#ifdef WIN2K
    edt_get_kernel_event(pdv_p, EDT_EVENT_BUF);
#endif

    /*
     * acquire the image
     */
    char buffer[32];
	snprintf(buffer, sizeof(char) * 32, "TimeCompPRC_%s.bin",fileName);  ///time component
	FILE *file = fopen(buffer, "wb");
    char bufferdiff[32];
	snprintf(bufferdiff, sizeof(char) * 32, "TimeCompDiffPRC_%s.bin",fileName);  ///time component
	FILE *filediff = fopen(bufferdiff, "wb");
    char buffer2[32];
	snprintf(buffer2, sizeof(char) * 32, "XjwPRC_%s.bin",fileName);  ///xjw
        FILE *flep;
        flep=fopen(buffer2, "wb");   // Xjw values
    char buffer3[32];
	snprintf(buffer3, sizeof(char) * 32, "stimPRC_%s.bin",fileName);  ///stim
        FILE *stimData;
        stimData=fopen(buffer3, "wb");   // stim data
    char buffer4[32];
	snprintf(buffer4, sizeof(char) * 32, "phasePRC_%s.bin",fileName);  ///phase
        FILE *phsData;
        phsData=fopen(buffer4, "wb");   // phase data
    char buffer5[32];
	snprintf(buffer5, sizeof(char) * 32, "prcPRC_%s.bin",fileName);  ///prc
        FILE *prcData;
        prcData=fopen(buffer5, "wb");   // phase data
    char buffer6[32];
	snprintf(buffer6, sizeof(char) * 32, "framesPRC_%s.bin",fileName);  ///raw frames
        FILE   *tmpf; 
        tmpf = fopen(buffer6, "wb"); // frames

       
 
    
	     
              //  FILE   *tmpf;                
               // tmpf = fopen("test.bin", "wb");
   
   uint16_t dark_array[65536];
   int pxls;
    //struct threeNum num;
   FILE *rddrk;
   if ((rddrk = fopen("frames_darkSnapsht.bin","rb")) == NULL){
       printf("Error! opening file");
       // Program exits if the file pointer returns NULL.
       exit(1);
   }
     fread(dark_array,sizeof(dark_array),1,rddrk); 
     
   //for(pxls = 0; pxls < 65536; ++pxls)
   //{
       
    //   printf("-----%d\n ", dark_array[pxls]);
   //}
   fclose(rddrk); 




	for( aa = 0; aa < nf; aa = aa + 1 ){
        
      
    
    {
     
        char header_size[32];
        if (pdv_get_header_size(pdv_p))
            sprintf(header_size,"hdr %d", pdv_get_header_size(pdv_p));
        else
            header_size[0] = 0;

        /* if (dmasize != imagesize)
            take_printf("reading image from %s\nwidth %d height %d depth %d %s total bytes %d (DMA %d)\n",
                    pdv_get_cameratype(pdv_p),
                    pdv_get_width(pdv_p),
                    pdv_get_height(pdv_p),
                    pdv_get_depth(pdv_p),
                    header_size,
                    imagesize,
                    dmasize);
        else
            take_printf("reading image from %s\nwidth %d height %d depth %d %s total bytes %d\n and %d\n dododo!",
                    pdv_get_cameratype(pdv_p),
                    pdv_get_width(pdv_p),
                    pdv_get_height(pdv_p),
                    pdv_get_depth(pdv_p),
                    header_size,
                    imagesize,
                    image_p); */
       
    }

    if (do_ring)
    {
        if (pdv_p->dd_p->force_single)
        {
            pdv_start_image(pdv_p);
            started = 1;
        }
        else if (do_lasttest)
        {
            /* start freerun */
            pdv_start_images(pdv_p, 0);
        }
        else if (wait_for_enter)
        {
            printf("hit enter for first image");
            getchar();
            pdv_start_images(pdv_p, 1);
            started = 1;
        }
        else
        {
            pdv_start_images(pdv_p, numbufs);
            started = numbufs;
        }
    }
    if (do_lasttest)
    {
        for (;;)
        {
            int     ret;
            u_char  tmpbuf[80];

#if 0
            printf("return to check");
            getchar();
#endif
            take_printf("%d done count %x\n", edt_done_count(pdv_p), edt_get_bytecount(pdv_p));
            edt_send_msg(pdv_p, 0, "w a07a 6", 8);
            ret = edt_serial_wait(pdv_p, 1000, 0);
            if (ret > 0)
            {
                take_printf("got %d resp\n", ret);
                edt_get_msg(pdv_p, (char *) tmpbuf, sizeof(tmpbuf));
                take_printf("resp %x %x %x\n", tmpbuf[0], tmpbuf[1], tmpbuf[2]);
                if (tmpbuf[2])
                {
                    printf("got %s - return to continue", &tmpbuf[2]);
                    getchar();
                }
            }
            else
            {
                printf("bad resp from serial wait %d\n", ret);
                getchar();
            }
            edt_send_msg(pdv_p, 0, "w a07a 4", 8);
            ret = edt_serial_wait(pdv_p, 1000, 0);
            if (ret > 0)
            {
                printf("got %d resp\n", ret);
                edt_get_msg(pdv_p, (char *) tmpbuf, sizeof(tmpbuf));
                printf("resp %x %x %x\n", tmpbuf[0], tmpbuf[1], tmpbuf[2]);
                if (tmpbuf[2])
                {
                    printf("got %s - return to continue", &tmpbuf[2]);
                    getchar();
                }
            }
            else
            {
                printf("bad resp from serial wait %d\n", ret);
                getchar();
            }
        }
    }
    done = 0;
    (void) edt_dtime();		/* init time for check */
    for (lc = 0; lc < loops && !GetOut; lc++)
    {
        if (wait_for_enter)
        {
            printf("hit enter for next image");
            getchar();
        }

        if (do_ring)
        {

            if (do_timestamp)
            {
                imagebuf = pdv_wait_image_timed(pdv_p, timestamp);
                switch (timetype)
                {
                    case EDT_TM_SEC_NSEC:
                        /* secs, nsec */
                        curtime = (double) timestamp[0] * 1000000000L + timestamp[1];
                        curtime /= 1000000000.0;
                        break;
                    case EDT_TM_CLICKS:
                        /* cpu cycles high32/low32 */
                        curtime = (double) timestamp[0] * FOURGB + timestamp[1];
                        break;
                    case EDT_TM_COUNTER:
                        /* counter high32/low32 */
                        curtime = (double) timestamp[0] * FOURGB + timestamp[1];
                        curtime /= frequency;
                        break;
                }
                take_printf("\ntime %3.9f ", curtime);
                if (svtime > 0)
                {
                    take_printf(" delta %3.9f ", (curtime - svtime));
                }
                svtime = curtime;

            }
            else
                imagebuf = pdv_wait_image(pdv_p);
            ++waited;
        }
        else
        {
            if (wait_for_enter && done == 0)
            {
                printf("hit enter for first image");
                getchar();
            } 

            bytesread = pdv_read(pdv_p, imagebuf, dmasize);

            if (bytesread != dmasize)
            {
                printf("dmasize 0x%x read 0x%x\n", dmasize, bytesread);
            }
            if (bytesread < 0)
            {
                pdv_perror(edt_devname);
                exit(1);
            }
        }
        if (checkfrm)
            pdv_checkfrm(pdv_p, (u_short *) imagebuf, width * height, verbose);
        timeouts = pdv_timeouts(pdv_p);
        if ((overrun = (edt_reg_read(pdv_p, PDV_STAT) & PDV_OVERRUN)))
        {
            ++overruns;
        }

        if (timeouts > last_timeouts)
        {
            bytesread = edt_get_timeout_count(pdv_p);
            if (debug_timeout)
            {
                FILE   *tmpf;

                tmpf = fopen("timeout.raw", "wb");
                if (tmpf)
                {
                    //printf("saving timeout data\n");
                    fwrite(imagebuf, imagesize, 1, tmpf);
                    fclose(tmpf);
                }
                printf("got timeout xfer %d - return to continue: ",
                        edt_get_bytecount(pdv_p));
                getchar();
            }

            /*
             * pdv_timeout_cleanup recovers from the timeout, in particular
             * when you've prestarted multiple buffers returns how many
             * buffers were pending before the reset so we can restart that
             * many again
             */
            if (do_ring)
            {
                pdv_timeout_restart(pdv_p,TRUE);
                in_timeout_cleanup = TRUE;
            }

            last_timeouts = timeouts;
            rbtimeout = 1;

        }
        else if (in_timeout_cleanup)
        {
            pdv_timeout_restart(pdv_p,TRUE);
            in_timeout_cleanup = FALSE;
        }

        done++;

        if (verbose)
        {
            int     j;
            u_char *tmp_p;
            int     lastval = 0;

            tmp_p = imagebuf;

            for (j = 0; j < 16; j++)
                take_printf("%02x ", imagebuf[j]);
            take_printf("\n");
            for (j = imagesize - 16; j < imagesize; j++)
                take_printf("%02x ", imagebuf[j]);
            take_printf("\n");
            if (checkclk)
            {
                for (j = 0; j < 512; j++)
                {
                    static int k = 0;
                    u_short tmpval = tmp_p[1] << 8 | tmp_p[0];

                    if (tmpval != lastval + 1)
                        printf("*");
                    else
                        printf(" ");
                    lastval = tmpval;

                    printf("%04x", tmpval);

                    if ((++k % 16) == 0)
                        printf("\n");
                    tmp_p += 4;
                }
                printf("\n");
            }
        }
        /*
         * check signature pixels from PCI CD simulator if -p or -P flag
         * specified -- expects first and last pixel to be count if -p, or
         * 0xA5 and 0x5A,  if -P, bombs out on first frame where this is not
         * the case
         */
        if (checksig || checkcount || checksim || checkclsim)
        {
            u_short bitmask = 0;

            for (i = 0; i < s_depth; i++)
                bitmask += 1 << i;

            /*
             * signature counter -- pick up the first one we come to
             */
            if (checkcount)
            {
                if (!siginit)
                {
                    startsig = endsig = sigcount = ((u_short *) imagebuf)[0] & bitmask;
                    siginit = 1;
                }
                else
                    startsig = endsig = sigcount & bitmask;
                ++sigcount;
            }
            if (checksim)
            {
                if (!siginit)
                {
                    u_short *sp = (u_short *) imagebuf;

                    startsig = sp[0];
                    endsig = sp[(imagesize / 2) - 1];
                    siginit = 1;
                }
                else
                {
                    startsig ^= 0xffff;
                    endsig ^= 0xffff;
                }
            }
            if (checkclsim)
            {
                if (!siginit)
                {
                    u_short *sp = (u_short *) imagebuf;

                    startsig = sp[0];
                    /* find 3333 */
                    if (sp[1] == 0x3333)
                        printf("init start  to %04x\n", startsig) ;
                    else
                        printf("buf is already not aligned\n") ;

                    siginit = 1;
                }
                else
                {
                    if (startsig == 0x7fff) startsig = 0 ;
                    else startsig = startsig + 1 ;
                }
            }


            if (depth > 8 || checkclsim)
            {
                u_short *sp = (u_short *) imagebuf;
                u_short firstword = sp[0];

                /*
                 * -4 because of one test xilinx which went into grid mode
                 * when fifo empty
                 */
                u_short lastword = sp[(imagesize / 2) - 4];

                if (checkclsim)
                {
                    u_int frmcnt = pdv_cl_get_fv_counter(pdv_p) ;
                    int k ;

                    if (firstword != startsig)
                        badstart = 1;
                    k = 0 ;
                    /* deal with 3333rd image */
                    if (sp[0] == 0x3333 && sp[1] == 0x3333) k = 1 ;
                    for (; k < imagesize/2 ; k++)
                    {
                        if (sp[k] == 0x3333)
                        {
                            if (k != 1)
                            {
                                printf("image1 3333 at %x\n",k) ;
                                badend = 1 ;
                            }
                            printf("frm %x count %04x stat %04x reg %08x\n",
                                    frmcnt,
                                    sp[k-1],
                                    edt_reg_read(pdv_p,PDV_STAT),
                                    edt_reg_read(pdv_p,PDV_CL_JERRYDBG)) ;
                            break ;
                        }
                    }
                }
                else
                {
                    if (firstword != startsig)
                        badstart = 1;
                    if (lastword != endsig)
                        badend = 1;
                }

                if (badstart || badend)
                {
                    u_char **buf = edt_buffer_addresses(pdv_p);

                    printf("\n\nstat %04x reg %08x\n",
                            edt_reg_read(pdv_p,PDV_STAT),
                            edt_reg_read(pdv_p,PDV_CL_JERRYDBG)) ;

                    /*
                     * if we just missed a frame, report it and go on
                     */
                    if (checksim
                            && (firstword == (startsig ^ 0xffff))
                            && (lastword == (endsig ^ 0xffff)))
                    {
                        printf("\nMISSED a frame! start %04x s/b %04x, end %04x s/b %04x\n", firstword, startsig, lastword, endsig);
                        printf("buffers %d donecount %d PDV_STAT %02x\n",
                                waited, edt_done_count(pdv_p), edt_reg_read(pdv_p, PDV_STAT));
                        startsig ^= 0xffff;
                        endsig ^= 0xffff;
                    }
                    else
                    {
                        printf("\nbad signature! start %04x s/b %04x, end %04x s/b %04x\n",
                                firstword, startsig, lastword, endsig);
                        printf("bad buf addr at %p %04x %04x\n", sp, firstword, lastword);
                        printf("buffers %d donecount %d PDV_STAT %02x\n",
                                waited, edt_done_count(pdv_p), edt_reg_read(pdv_p, PDV_STAT));

                        edt_stop_buffers(pdv_p);

                        sprintf(cmdstr,"setdebug -d 0 -u %d -g >setdebug.out\n",unit);
                        printf("%s\n",cmdstr) ;
                        system(cmdstr) ;


                        printf("dumping %d buffers to buf01.raw, buf02.raw, ....\n",
                                numbufs);
                        for (i = 0; i < numbufs; i++)
                        {
                            char    bufname[32];

                            sprintf(bufname, "buf%02d.raw", i);
                            printf("dump buf %d from %p\n", i, buf[i]);
                            dumpbuf(bufname, (u_short *) buf[i], imagesize);
                        }

                        printf("return to continue");
                        getchar();
                        if (!(checkcount||checkclsim))
                        {
                            for (i = 0; i < imagesize / 2; i++)
                                if (((sp[i] & bitmask) == startsig)
                                        || (sp[i] & bitmask) == endsig)
                                    printf("found %04x at %d\n", sp[i], i);
                        }
                        exit(1);
                    }
                }
                /*
                 * sp[0] = 0xaaaa ; sp[imagesize/2 - 1] = 0x5555 ;
                 */
            }
            else
            {
                u_char *cp = imagebuf;

                if (cp[0] != startsig)
                    badstart = 1;
                if (cp[imagesize - 1] != endsig)
                    badend = 1;
                if (badstart || badend)
                {
                    printf("\nbad signature! start %02x s/b %02x, end %02x s/b %02x\n",
                            cp[0], startsig, cp[imagesize-1], endsig);

                    if (!checkcount)
                    {
                        for (i = 0; i < imagesize; i++)
                            if ((cp[i] == startsig)
                                    || (cp[i] == endsig))
                                printf("found %02x at %d\n", cp[i], i);
                    }
                    exit(1);
                }
            }
        }
        if (do_ring && started < loops)
        {
            if (!do_lasttest)
            {
                pdv_start_images(pdv_p, 1);
            }
            started++;
        }

        /*
         * process subimage if needed, set up args for write
         */
        if (do_file && (subimage != NULL))
        {
            crop_and_subsample(subsample, width, height,
                    crop_x0, crop_y0, crop_x1, crop_y1, imagebuf, subimage);
            image_p = subimage;
        }
        else
            image_p = imagebuf;


        if (pdv_p->dd_p->fval_done)
        {	
            int lines = pdv_get_lines_xferred(pdv_p);
            s_size = s_width * lines * s_db;
        }

        /*
         * write the image to raw data file (no formatting)
         */
        sumsize += s_size;

        if (*rawfname)
        {
            resolve_fname(rawfname, tmpname, loops, ".raw");
            take_printf("writing %dx%dx%d raw file to %s (actual size %d)\n",
                    s_width, s_height, s_depth, tmpname, s_size);

            (void) dvu_write_raw(s_size, image_p, tmpname);
        }

#if DO_JPEG
        if (*jpgfname)
        {
            char filename[256];

            if ((strcmp(&jpgfname[strlen(jpgfname) - 4], ".jpg") == 0)
            || (strcmp(&jpgfname[strlen(jpgfname) - 4], ".JPG") == 0))
                jpgfname[strlen(jpgfname)] = '\0';
                sprintf(filename, "%s%02d.jpg", jpgfname, i);

            edt_save_jpeg(filename, width, height, bits2bytes(depth), 0, image_p);
        }
#endif

        /*
         * write the image to Windows bitmap format file and
         * fix up the filename extension
         */
        if (*bmpfname && !mosaic)
        {
		
	    //int s_db = bits2bytes(s_depth);
	    //int pixcount = 0;
		//char buffer[32];
		//snprintf(buffer, sizeof(char) * 32, "file%i.txt", aa);
		////FILE *file = fopen(buffer, "wb");
		//DataArray = (u_char**) malloc(s_height*sizeof(u_char *));
                //int rows;
		//for(rows=0; rows<s_height; rows++) DataArray[rows]=(u_char *) malloc(s_width*sizeof(u_char));

                //if (tmpf)
               // {
                    //printf("saving timeout data\n");
                imagebuf16 = (uint16_t *) imagebuf;
                    for(i=0; i<65536; i++){
                                
				imagebuf16[i]*=weightArr[i];
                                difframe=(int)imagebuf16[i]-(int)dark_array[i];
                                //printf("%d------%d\n",aa, difframe);
				//printf("--%u\n",imagebuf16[i]);
				timecomp[aa][0]+=imagebuf16[i];
                                timecompdiff[aa][0]+=difframe;
                                //printf("--%f\n",timecomp[aa][0]);

                                if (clmt[i]==1.000000){
                                timecomp[aa][1]+=imagebuf16[i];
                                timecompdiff[aa][1]+=difframe;
                                }
                                else if (clmt[i]==2.000000){
                                timecomp[aa][2]+=imagebuf16[i];
                                timecompdiff[aa][2]+=difframe;
                                }
				else if (clmt[i]==3.000000){
                                timecomp[aa][3]+=imagebuf16[i];
                                timecompdiff[aa][3]+=difframe;
                                }
				else if (clmt[i]==4.000000){
                                timecomp[aa][4]+=imagebuf16[i];
                                timecompdiff[aa][4]+=difframe;
                                }


			}
                    
                   fwrite(imagebuf16, imagesize, 1, tmpf);  // writing binary data
                    
                    //fprintf(file,"%f\n",(timecomp[aa]));
		    //fprintf(file," ");
                    //printf("--------------%g+%gi\t",creal(Xjw[aa]),cimag(Xjw[aa]));
                    //printf("--------------------%f",timecomp[aa]);
                  
		    //fread(fid, weightbuf....

                    //printf("imagebuf is\n");
                    //printf("%d",sizeof(imagebuf));
			//timecomp[k]=0;
			//(file," ");
                    //printf("Value at imagebuf=%u",(uint16_t)imagebuf[150]);
                    //printf(".\n");
                    //fclose(tmpf);
                //}


       struct timeval end_t, start_t;
       gettimeofday(&start_t, NULL);

        
        DAQmxErrChk (DAQmxWriteAnalogF64(taskHandle,1,1,10.0,DAQmx_Val_GroupByChannel,&stim[aa],NULL,NULL));

        DAQmxStopTask(taskHandle);
        DAQmxErrChk (DAQmxWriteAnalogF64(taskHandle,1,1,10.0,DAQmx_Val_GroupByChannel,&stim[0],NULL,NULL));

        DAQmxStopTask(taskHandle);
        //printf("%d\n",aa);

        gettimeofday(&end_t, NULL);
       // printf(" start %lu us\n", start_t.tv_usec);
       // printf(" end %lu us \n", end_t.tv_usec);
       // printf(" took %lu us \n", (end_t.tv_usec - start_t.tv_usec));
        //fprintf(stimData,"%f\n",stim[aa]);
        
        //printf("End of program, press Enter key to quit\n");
        
        
        
/*
	    for(rows=0; rows< s_height; rows++){
                        int cols;
			for(cols = 0; cols<s_width; cols++){
				DataArray[rows][cols]=*(image_p+pixcount*s_db);
				fprintf(file,"%d",DataArray[rows][cols]);
				fprintf(file," ");
				pixcount++;
			}
			//printf("\n");
			
	    }
	    fprintf(file,"\n");
	    */
        
        
		    //char straa[128];
		   // sprintf(straa, "%i", aa);
            //resolve_fname(straa, tmpname, loops, ".bmp");
            //if (write_bmp_file(tmpname, image_p,
                    //s_width, s_height, s_depth) != 0)
                      //  printf("file write failed\n");
		
			
        
			
        }
        
        
        
        else if (mosaic)
        {
            memcpy(mosaic_ptr, image_p, mosaic_sample);
            mosaic_ptr += mosaic_sample;
        }

        dl = pdv_debug_level();

        /*if (rbtimeout)
        {
            if (bytesread == 0)
                take_printf("%d images %d timeouts %d overruns 0 bytes", done, timeouts, overruns);
            else take_printf("%d images %d timeouts %d overruns %d bytes (%d short)", done, timeouts, overruns, bytesread, dmasize-bytesread);
        }
        else if (just_print_the_last_one)
        {
            if (lc == loops-1)
                take_printf("%d images %d timeouts %d overruns\n", done, timeouts, overruns);
        }
        else take_printf("%d images %d timeouts %d overruns", done, timeouts, overruns); */

        if (!just_print_the_last_one)
        {
            if (verbose | rbtimeout | wait_for_enter | (pdv_debug_level() > 0))
                take_printf("\n");
            else take_printf("                                \r");
        }

        rbtimeout = 0;
        fflush(stdout);
    }
}

    fwrite(timecomp,sizeof(timecomp),1,file); 
    fwrite(timecompdiff,sizeof(timecompdiff),1,filediff);
    fwrite(Xjw, sizeof(Xjw), 1, flep);
    fwrite(phase, sizeof(phase), 1, phsData);
    fwrite(prc, sizeof(prc), 1, prcData);
    fwrite(stim, sizeof(stim), 1, stimData);
    fclose(tmpf);
    fclose(file);
     fclose(filediff);
    take_printf("\n");
    fclose(flep);
    fclose(stimData);
    fclose(phsData);
    fclose(prcData);
    if (mosaic && *bmpfname)
    {
        if (write_bmp_file(bmpfname, mosaic_image,
                s_width, mosaic_height, s_depth) != 0)
                        printf("file write failed\n");
    }
   
    if (loops > 3)
    {
        dtime = edt_dtime();
        printf("%lf bytes/sec\n", sumsize / dtime);
        printf("%lf frames/sec\n", (double) (loops) / dtime);
    }

    if (!do_ring)
        pdv_free(imagebuf);
    /* else */
    pdv_stop_continuous(pdv_p);

    if (binning)
    {
        pdv_set_binning(pdv_p, 1, 1);
    }

    pdv_close(pdv_p);
#ifdef NO_MAIN
    if (overruns || timeouts)
        return (2);
    return (0);
#else
    if (overruns || timeouts)
        exit(2);
    exit(0);
#endif
Error:
	if( DAQmxFailed(error) )
		DAQmxGetExtendedErrorInfo(errBuff,2048);
	if( taskHandle!=0 ) {
		/*********************************************/
		// DAQmx Stop Code
		/*********************************************/
		DAQmxStopTask(taskHandle);
		DAQmxClearTask(taskHandle);
	}
	if( DAQmxFailed(error) )
		printf("DAQmx Error: %s\n",errBuff);
   
     
}





    void
resolve_fname(char *fname, char *newname, int loops, char *ext)
{
    int     l = (int)strlen(fname);
    char    tmpname[256];
    char    tmpext[5];

    strcpy(tmpname, fname);
    if ((l > 4) && (tmpname[l - 4] == '.'))
    {
        strcpy(tmpext, &tmpname[l - 4]);
        tmpname[l - 4] = '\0';
    }
    else
        strcpy(tmpext, ext);

    if (loops > 1)
    {
        sprintf(newname, "%s%04d%s", tmpname, ASnum, tmpext);
        ASnum += ASinc;
    }
    else
        sprintf(newname, "%s%s", tmpname, tmpext);
}


/*
 * process the image, return in subimage. Handles both subsampling and
 * cropping.
 * 
 * NOTE: this only works on single-byte images (so far)
 */

    void
crop_and_subsample(int subsample, int src_width, int src_height,
        int x0, int y0, int x1, int y1, u_char * src, u_char * dst)
{
    u_char *next_fromptr = src + (y0 * src_width) + x0;
    int     to_width = (x1 - x0) + 1;
    int     lines = (y1 - y0) + 1;
    u_char *to = dst;
    u_char *from;
    u_char *from_end;

    while (lines > 0)
    {
        from = (u_char *) next_fromptr;
        from_end = from + to_width;

        while (from < from_end)
        {
            *to++ = *from;
            from += subsample;
        }
        next_fromptr += src_width * subsample;
        lines -= subsample;
    }
}


    void
usage(char *progname, char *errmsg, int exitval)
{
    printf("\n");
    printf("%s", errmsg);
    printf("\n%s: Image acquisition and optional save for EDT PCI DV family digital\n", progname);
    printf("DMA interface products. Many options make it useful for diagnostics and\n");
    printf("also as an example for various methods of doing acquisition with EDT PCI DV\n");
    printf("family products. For a much a simpler example, see simple_take.c\n\n");
    printf("usage: %s\n", progname);
    printf("  -e exposuretime   - set exposure time before acquiring (if supported)\n");
    printf("  -g gain           - set camera gain (if supported)\n");
    printf("  -o offset         - set camera offset (if supported)\n");
    printf("  -l loopcount      - loop for loopcount images\n");
    printf("  -m msecs          - delay msecs milliseconds between images\n");
    printf("  -f out_fname      - output to raw file(s)\n");
    printf("  -b fname.bmp      - output to Microsoft bitmap file(s)\n");
#ifdef DO_JPEG
    printf("  -j fname.jpg      - output to jpeg format file(s)\n");
#endif
    printf("  -C x0 y0 x1 y1    - crop the output image to the coordinates given\n");
    printf("  -N n              - number of multiple buffers (default 4, 0 forces pdv_read()\n");
    printf("  -r [hs ha vs va]  - region of interest -- to disable use -r alone (no subargs)\n");
    printf("  -t [msecs]        - timeout # of msecs. if 0, disables timeouts, if no subarg,\n");
    printf("  -T                - if image times out, dump any recieved data to timeout.raw\n");
    printf("                        re-enables auto timeouts\n");
    printf("  -w                - wait for <CR> between images\n");
    printf("  -s start inc      - specify start and increment for auto-numbered file-\n");
    printf("                        names (use with -l of 2 or more for auto-numbered filenames)\n");
    printf("  -I inc            - specify increment for auto-numbered file (see -s)\n");
    printf("  -v                - verbose output\n");
    printf("  -q binval         - set binning (applicable cameras only)\n");
    printf("  -p                - check pci dv simulator signature, exit if bad\n");
    printf("  -d                - get and print a driver timestamp with every image\n");
    printf("  -J                - just output the last take line (for output to file)\n");
    printf("  -c channel        - channel # if multiple channels (2nd base Camera Link\n");
    printf("                        connector or multiple RCI units on one FOI card (default 0)\n");
    printf("  -u unit           - pdv unit number (default 0)\n");
    printf("                        A full device pathname can also be used\n");
    printf("  -B                - turn off burst enable\n");
    printf("  -X                - use timer\n");
    printf("  -L                - freerun/bytecount with wait for next frame\n");
    printf("  -H                - check count signature from testgen\n");
    printf("  -P                - check count signature from PCI CD sim\n");
    printf("  -G                - check count signature from testgen\n");
    printf("  -Q                - turn on quiet mode\n");
    printf("  -n                - autosequence start only\n");
    printf("  -R                - disable ROI\n");
    printf("  -O                - try noreset - will be obsolete on clink?\n");
    printf("  -x                - experimental - strobe testing\n");
    printf("  -M                - mosaic\n");
    printf("  -a                - call pdv_checkfrm\n");
    printf("  -D                - dvc mode setting - diagnostic\n");
    printf("  -y                - turns on triggering through the optical coupler\n");
    printf("  -Y                - turns on triggering via the field id\n");
    printf("  -h                - this help message\n");
    exit(exitval);
}

