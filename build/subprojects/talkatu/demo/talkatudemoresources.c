#include <gio/gio.h>

#if defined (__ELF__) && ( __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 6))
# define SECTION __attribute__ ((section (".gresource.talkatudemo"), aligned (8)))
#else
# define SECTION
#endif

#ifdef _MSC_VER
static const SECTION union { const guint8 data[2717]; const double alignment; void * const ptr;}  talkatudemo_resource_data = { {
  0107, 0126, 0141, 0162, 0151, 0141, 0156, 0164, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 
  0030, 0000, 0000, 0000, 0034, 0001, 0000, 0000, 0000, 0000, 0000, 0050, 0011, 0000, 0000, 0000, 
  0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0004, 0000, 0000, 0000, 0006, 0000, 0000, 0000, 
  0006, 0000, 0000, 0000, 0006, 0000, 0000, 0000, 0010, 0000, 0000, 0000, 0010, 0000, 0000, 0000, 
  0011, 0000, 0000, 0000, 0006, 0074, 0270, 0052, 0005, 0000, 0000, 0000, 0034, 0001, 0000, 0000, 
  0005, 0000, 0114, 0000, 0044, 0001, 0000, 0000, 0050, 0001, 0000, 0000, 0320, 0061, 0002, 0041, 
  0010, 0000, 0000, 0000, 0050, 0001, 0000, 0000, 0010, 0000, 0114, 0000, 0060, 0001, 0000, 0000, 
  0064, 0001, 0000, 0000, 0113, 0120, 0220, 0013, 0006, 0000, 0000, 0000, 0064, 0001, 0000, 0000, 
  0004, 0000, 0114, 0000, 0070, 0001, 0000, 0000, 0074, 0001, 0000, 0000, 0075, 0237, 0024, 0265, 
  0001, 0000, 0000, 0000, 0074, 0001, 0000, 0000, 0003, 0000, 0114, 0000, 0100, 0001, 0000, 0000, 
  0104, 0001, 0000, 0000, 0042, 0042, 0032, 0100, 0007, 0000, 0000, 0000, 0104, 0001, 0000, 0000, 
  0007, 0000, 0166, 0000, 0120, 0001, 0000, 0000, 0155, 0012, 0000, 0000, 0122, 0134, 0116, 0150, 
  0002, 0000, 0000, 0000, 0155, 0012, 0000, 0000, 0012, 0000, 0114, 0000, 0170, 0012, 0000, 0000, 
  0174, 0012, 0000, 0000, 0324, 0265, 0002, 0000, 0377, 0377, 0377, 0377, 0174, 0012, 0000, 0000, 
  0001, 0000, 0114, 0000, 0200, 0012, 0000, 0000, 0204, 0012, 0000, 0000, 0061, 0200, 0145, 0173, 
  0003, 0000, 0000, 0000, 0204, 0012, 0000, 0000, 0005, 0000, 0114, 0000, 0214, 0012, 0000, 0000, 
  0220, 0012, 0000, 0000, 0153, 0354, 0170, 0204, 0000, 0000, 0000, 0000, 0220, 0012, 0000, 0000, 
  0010, 0000, 0114, 0000, 0230, 0012, 0000, 0000, 0234, 0012, 0000, 0000, 0153, 0145, 0145, 0160, 
  0057, 0000, 0000, 0000, 0010, 0000, 0000, 0000, 0164, 0141, 0154, 0153, 0141, 0164, 0165, 0057, 
  0003, 0000, 0000, 0000, 0157, 0162, 0147, 0057, 0005, 0000, 0000, 0000, 0165, 0151, 0057, 0000, 
  0007, 0000, 0000, 0000, 0144, 0145, 0155, 0157, 0056, 0165, 0151, 0000, 0000, 0000, 0000, 0000, 
  0276, 0161, 0000, 0000, 0001, 0000, 0000, 0000, 0170, 0332, 0355, 0135, 0133, 0163, 0342, 0072, 
  0022, 0176, 0347, 0127, 0150, 0375, 0164, 0116, 0355, 0070, 0044, 0231, 0275, 0114, 0355, 0020, 
  0316, 0222, 0014, 0111, 0250, 0311, 0255, 0200, 0324, 0124, 0236, 0050, 0141, 0013, 0133, 0033, 
  0141, 0371, 0110, 0002, 0302, 0326, 0376, 0370, 0155, 0331, 0046, 0201, 0140, 0203, 0315, 0045, 
  0201, 0104, 0057, 0251, 0140, 0267, 0244, 0276, 0175, 0255, 0126, 0133, 0262, 0053, 0177, 0074, 
  0365, 0031, 0032, 0022, 0041, 0051, 0017, 0116, 0254, 0243, 0203, 0103, 0013, 0221, 0300, 0341, 
  0056, 0015, 0274, 0023, 0353, 0276, 0175, 0156, 0177, 0263, 0376, 0250, 0226, 0052, 0177, 0261, 
  0155, 0164, 0101, 0002, 0042, 0260, 0042, 0056, 0032, 0121, 0345, 0043, 0217, 0141, 0227, 0240, 
  0257, 0007, 0137, 0277, 0035, 0034, 0243, 0122, 0251, 0215, 0331, 0043, 0126, 0003, 0004, 0164, 
  0355, 0237, 0100, 0341, 0172, 0104, 0111, 0324, 0343, 0002, 0071, 0076, 0126, 0010, 0207, 0041, 
  0243, 0016, 0126, 0060, 0214, 0054, 0235, 0361, 0160, 0054, 0250, 0347, 0053, 0364, 0333, 0331, 
  0357, 0350, 0370, 0360, 0350, 0237, 0366, 0361, 0341, 0361, 0041, 0272, 0300, 0142, 0214, 0176, 
  0012, 0334, 0007, 0122, 0037, 0125, 0074, 0101, 0373, 0377, 0026, 0004, 0207, 0104, 0214, 0270, 
  0140, 0356, 0201, 0303, 0373, 0125, 0030, 0311, 0247, 0022, 0205, 0202, 0173, 0100, 0210, 0340, 
  0337, 0236, 0040, 0004, 0111, 0336, 0123, 0043, 0054, 0310, 0167, 0064, 0346, 0003, 0344, 0340, 
  0000, 0011, 0342, 0122, 0251, 0004, 0355, 0016, 0024, 0101, 0024, 0070, 0010, 0334, 0062, 0160, 
  0323, 0007, 0331, 0172, 0343, 0022, 0134, 0030, 0004, 0056, 0021, 0110, 0371, 0004, 0051, 0042, 
  0372, 0022, 0361, 0136, 0364, 0343, 0342, 0346, 0076, 0021, 0225, 0241, 0273, 0101, 0027, 0130, 
  0101, 0127, 0324, 0041, 0201, 0044, 0010, 0303, 0270, 0372, 0212, 0364, 0101, 0011, 0335, 0161, 
  0111, 0223, 0237, 0353, 0321, 0133, 0311, 0350, 0350, 0234, 0103, 0257, 0221, 0224, 0337, 0021, 
  0001, 0055, 0301, 0000, 0211, 0162, 0321, 0361, 0144, 0200, 0244, 0267, 0057, 0210, 0213, 0322, 
  0157, 0240, 0032, 0140, 0130, 0040, 0036, 0352, 0106, 0277, 0003, 0227, 0143, 0304, 0100, 0311, 
  0317, 0355, 0016, 0346, 0005, 0176, 0221, 0313, 0105, 0064, 0210, 0372, 0364, 0171, 0010, 0142, 
  0150, 0105, 0203, 0140, 0043, 0312, 0030, 0352, 0022, 0064, 0220, 0244, 0067, 0140, 0137, 0112, 
  0100, 0211, 0176, 0065, 0332, 0227, 0267, 0367, 0155, 0124, 0273, 0171, 0100, 0277, 0152, 0315, 
  0146, 0355, 0246, 0375, 0360, 0075, 0062, 0044, 0207, 0273, 0144, 0110, 0342, 0176, 0150, 0037, 
  0314, 0244, 0055, 0214, 0205, 0300, 0201, 0032, 0003, 0317, 0245, 0353, 0172, 0363, 0354, 0022, 
  0350, 0153, 0247, 0215, 0253, 0106, 0373, 0001, 0330, 0106, 0347, 0215, 0366, 0115, 0275, 0325, 
  0102, 0347, 0267, 0115, 0124, 0103, 0167, 0265, 0146, 0273, 0161, 0166, 0177, 0125, 0153, 0242, 
  0273, 0373, 0346, 0335, 0155, 0253, 0176, 0200, 0120, 0213, 0150, 0206, 0110, 0151, 0201, 0072, 
  0173, 0221, 0071, 0100, 0153, 0056, 0121, 0230, 0062, 0011, 0242, 0076, 0200, 0361, 0044, 0260, 
  0304, 0134, 0344, 0343, 0041, 0001, 0043, 0072, 0204, 0016, 0201, 0041, 0214, 0034, 0160, 0232, 
  0345, 0066, 0052, 0141, 0306, 0003, 0057, 0366, 0120, 0245, 0365, 0306, 0150, 0127, 0200, 0133, 
  0175, 0107, 0264, 0207, 0002, 0256, 0276, 0040, 0011, 0214, 0125, 0174, 0245, 0102, 0371, 0257, 
  0162, 0171, 0064, 0032, 0035, 0170, 0301, 0340, 0200, 0013, 0257, 0314, 0342, 0036, 0144, 0271, 
  0012, 0214, 0330, 0066, 0170, 0075, 0015, 0300, 0020, 0075, 0354, 0220, 0152, 0011, 0241, 0212, 
  0040, 0177, 0016, 0250, 0040, 0121, 0227, 0047, 0126, 0342, 0357, 0326, 0013, 0170, 0016, 0001, 
  0074, 0345, 0024, 0112, 0117, 0075, 0376, 0165, 0212, 0354, 0353, 0301, 0321, 0267, 0204, 0116, 
  0243, 0352, 0171, 0014, 0073, 0031, 0337, 0126, 0143, 0260, 0245, 0027, 0262, 0341, 0061, 0322, 
  0134, 0314, 0321, 0005, 0270, 0117, 0320, 0063, 0334, 0322, 0050, 0134, 0042, 0035, 0101, 0043, 
  0217, 0132, 0216, 0306, 0364, 0056, 0234, 0147, 0210, 0346, 0001, 0345, 0244, 0017, 0105, 0300, 
  0177, 0300, 0173, 0221, 0303, 0260, 0224, 0317, 0112, 0372, 0101, 0372, 0374, 0027, 0015, 0134, 
  0076, 0262, 0120, 0010, 0060, 0011, 0324, 0211, 0165, 0241, 0036, 0223, 0113, 0272, 0045, 0264, 
  0005, 0007, 0207, 0076, 0301, 0345, 0264, 0174, 0047, 0026, 0100, 0330, 0356, 0161, 0147, 0040, 
  0255, 0352, 0071, 0146, 0222, 0124, 0312, 0023, 0202, 0164, 0372, 0056, 0027, 0000, 0147, 0033, 
  0104, 0125, 0276, 0125, 0075, 0072, 0236, 0243, 0227, 0324, 0013, 0300, 0135, 0142, 0152, 0227, 
  0060, 0242, 0210, 0255, 0375, 0136, 0131, 0340, 0152, 0201, 0313, 0210, 0070, 0261, 0124, 0314, 
  0157, 0307, 0005, 0206, 0073, 0243, 0210, 0275, 0216, 0303, 0270, 0044, 0156, 0307, 0351, 0132, 
  0110, 0216, 0100, 0161, 0304, 0075, 0261, 0002, 0036, 0133, 0020, 0272, 0165, 0174, 0312, 0334, 
  0370, 0177, 0370, 0305, 0273, 0377, 0041, 0216, 0232, 0210, 0017, 0062, 0336, 0341, 0200, 0270, 
  0326, 0344, 0376, 0074, 0333, 0103, 0052, 0151, 0227, 0021, 0253, 0332, 0026, 0203, 0071, 0031, 
  0227, 0350, 0045, 0157, 0023, 0056, 0050, 0210, 0031, 0131, 0333, 0252, 0202, 0037, 0052, 0060, 
  0075, 0313, 0323, 0020, 0224, 0111, 0354, 0130, 0071, 0013, 0106, 0233, 0321, 0100, 0212, 0026, 
  0022, 0047, 0150, 0071, 0202, 0063, 0106, 0334, 0031, 0253, 0257, 0254, 0226, 0025, 0125, 0223, 
  0326, 0114, 0372, 0030, 0030, 0212, 0140, 0147, 0125, 0151, 0220, 0331, 0154, 0116, 0316, 0154, 
  0131, 0057, 0041, 0102, 0163, 0061, 0266, 0020, 0005, 0147, 0361, 0223, 0037, 0257, 0233, 0316, 
  0163, 0242, 0377, 0132, 0325, 0204, 0076, 0213, 0217, 0225, 0325, 0265, 0206, 0312, 0322, 0232, 
  0106, 0330, 0201, 0166, 0027, 0077, 0176, 0166, 0356, 0156, 0033, 0067, 0355, 0172, 0263, 0163, 
  0175, 0333, 0156, 0334, 0336, 0164, 0256, 0153, 0255, 0237, 0350, 0177, 0050, 0345, 0316, 0045, 
  0374, 0232, 0276, 0175, 0172, 0337, 0156, 0303, 0345, 0273, 0046, 0314, 0046, 0051, 0327, 0233, 
  0365, 0253, 0172, 0255, 0125, 0217, 0356, 0024, 0341, 0315, 0047, 0117, 0041, 0270, 0255, 0055, 
  0211, 0052, 0056, 0330, 0060, 0177, 0343, 0112, 0071, 0066, 0377, 0053, 0117, 0051, 0317, 0103, 
  0042, 0205, 0260, 0022, 0142, 0347, 0021, 0362, 0254, 0305, 0336, 0011, 0323, 0010, 0375, 0057, 
  0051, 0352, 0321, 0202, 0006, 0217, 0031, 0201, 0163, 0302, 0320, 0334, 0350, 0163, 0154, 0057, 
  0105, 0066, 0304, 0267, 0123, 0376, 0264, 0145, 0050, 0057, 0020, 0142, 0215, 0120, 0227, 0033, 
  0321, 0040, 0143, 0233, 0163, 0326, 0305, 0302, 0172, 0033, 0034, 0056, 0224, 0067, 0233, 0355, 
  0164, 0326, 0233, 0330, 0245, 0134, 0363, 0177, 0072, 0120, 0012, 0164, 0022, 0005, 0044, 0305, 
  0075, 0217, 0221, 0016, 0314, 0324, 0064, 0260, 0322, 0372, 0131, 0103, 0260, 0165, 0205, 0113, 
  0153, 0257, 0200, 0177, 0105, 0103, 0133, 0221, 0047, 0230, 0254, 0025, 0044, 0246, 0022, 0222, 
  0014, 0014, 0374, 0234, 0130, 0143, 0002, 0275, 0336, 0353, 0334, 0174, 0222, 0022, 0235, 0016, 
  0172, 0075, 0042, 0212, 0216, 0301, 0160, 0227, 0260, 0324, 0316, 0357, 0264, 0232, 0212, 0166, 
  0007, 0251, 0267, 0035, 0255, 0057, 0030, 0015, 0126, 0324, 0032, 0165, 0170, 0140, 0307, 0063, 
  0202, 0026, 0334, 0176, 0262, 0075, 0235, 0361, 0122, 0247, 0150, 0107, 0330, 0121, 0220, 0102, 
  0347, 0144, 0142, 0046, 0113, 0212, 0035, 0305, 0135, 0222, 0040, 0165, 0043, 0205, 0167, 0040, 
  0261, 0014, 0274, 0044, 0121, 0212, 0375, 0060, 0065, 0377, 0113, 0311, 0241, 0362, 0304, 0324, 
  0105, 0041, 0063, 0163, 0206, 0212, 0342, 0370, 0212, 0116, 0347, 0363, 0076, 0327, 0012, 0347, 
  0171, 0346, 0306, 0324, 0170, 0272, 0140, 0072, 0330, 0054, 0220, 0107, 0076, 0327, 0320, 0374, 
  0150, 0100, 0376, 0245, 0305, 0332, 0064, 0232, 0243, 0116, 0337, 0027, 0315, 0130, 0072, 0224, 
  0026, 0155, 0357, 0011, 0076, 0010, 0041, 0022, 0114, 0205, 0156, 0203, 0345, 0017, 0210, 0145, 
  0137, 0365, 0331, 0307, 0203, 0362, 0145, 0373, 0372, 0152, 0323, 0110, 0326, 0175, 0356, 0300, 
  0264, 0254, 0015, 0146, 0300, 0154, 0300, 0234, 0006, 0346, 0076, 0026, 0217, 0240, 0333, 0017, 
  0230, 0144, 0137, 0047, 0222, 0155, 0032, 0324, 0223, 0176, 0167, 0051, 0337, 0266, 0047, 0125, 
  0115, 0003, 0363, 0317, 0013, 0363, 0026, 0011, 0261, 0300, 0212, 0013, 0015, 0365, 0006, 0170, 
  0304, 0356, 0141, 0332, 0030, 0056, 0253, 0170, 0223, 0204, 0346, 0217, 0020, 0205, 0033, 0201, 
  0004, 0162, 0244, 0223, 0037, 0324, 0023, 0274, 0017, 0041, 0271, 0107, 0213, 0257, 0150, 0262, 
  0043, 0360, 0324, 0000, 0373, 0222, 0135, 0315, 0304, 0114, 0207, 0121, 0347, 0161, 0151, 0314, 
  0244, 0221, 0224, 0121, 0266, 0155, 0002, 0246, 0301, 0135, 0156, 0334, 0115, 0362, 0223, 0255, 
  0142, 0157, 0027, 0223, 0240, 0055, 0202, 0160, 0222, 0045, 0033, 0040, 0232, 0314, 0305, 0030, 
  0156, 0072, 0202, 0352, 0354, 0377, 0365, 0322, 0022, 0017, 0224, 0317, 0005, 0044, 0375, 0373, 
  0022, 0132, 0263, 0303, 0135, 0055, 0022, 0345, 0175, 0243, 0234, 0034, 0113, 0200, 0204, 0015, 
  0235, 0012, 0271, 0215, 0305, 0131, 0142, 0256, 0204, 0170, 0046, 0304, 0045, 0267, 0102, 0036, 
  0362, 0041, 0021, 0046, 0276, 0155, 0003, 0046, 0232, 0343, 0216, 0303, 0331, 0007, 0101, 0314, 
  0231, 0226, 0344, 0175, 0001, 0023, 0051, 0323, 0226, 0204, 0201, 0025, 0154, 0071, 0356, 0167, 
  0071, 0053, 0232, 0035, 0024, 0102, 0316, 0224, 0005, 0027, 0200, 0150, 0212, 0312, 0340, 0311, 
  0344, 0013, 0046, 0020, 0276, 0056, 0105, 0253, 0314, 0075, 0054, 0373, 0267, 0026, 0213, 0005, 
  0105, 0211, 0110, 0050, 0142, 0226, 0062, 0252, 0306, 0233, 0013, 0264, 0311, 0226, 0237, 0255, 
  0207, 0132, 0343, 0311, 0205, 0075, 0131, 0222, 0300, 0335, 0243, 0311, 0074, 0257, 0067, 0153, 
  0261, 0120, 0054, 0326, 0126, 0074, 0272, 0245, 0373, 0217, 0325, 0151, 0274, 0172, 0367, 0274, 
  0232, 0270, 0124, 0315, 0154, 0024, 0337, 0173, 0207, 0036, 0371, 0044, 0072, 0377, 0242, 0317, 
  0153, 0364, 0211, 0224, 0330, 0043, 0372, 0360, 0112, 0054, 0350, 0346, 0374, 0272, 0276, 0122, 
  0177, 0306, 0245, 0063, 0134, 0072, 0113, 0316, 0154, 0031, 0127, 0226, 0357, 0165, 0303, 0036, 
  0145, 0254, 0370, 0316, 0331, 0220, 0113, 0032, 0157, 0367, 0075, 0134, 0264, 0117, 0073, 0175, 
  0257, 0165, 0232, 0016, 0212, 0354, 0363, 0327, 0316, 0307, 0105, 0014, 0144, 0022, 0377, 0277, 
  0234, 0343, 0102, 0173, 0224, 0067, 0272, 0313, 0330, 0154, 0331, 0177, 0061, 0160, 0174, 0326, 
  0011, 0026, 0245, 0166, 0364, 0063, 0216, 0153, 0131, 0031, 0162, 0272, 0365, 0333, 0205, 0162, 
  0352, 0135, 0122, 0376, 0332, 0006, 0130, 0333, 0010, 0233, 0214, 0250, 0161, 0340, 0310, 0265, 
  0156, 0134, 0165, 0206, 0237, 0363, 0026, 0032, 0204, 0003, 0125, 0300, 0127, 0032, 0231, 0364, 
  0357, 0157, 0211, 0224, 0303, 0164, 0321, 0226, 0022, 0253, 0232, 0154, 0055, 0131, 0141, 0353, 
  0012, 0120, 0006, 0366, 0100, 0260, 0045, 0105, 0236, 0041, 0045, 0243, 0216, 0246, 0355, 0000, 
  0355, 0332, 0017, 0200, 0136, 0063, 0241, 0163, 0152, 0073, 0311, 0075, 0362, 0060, 0022, 0055, 
  0055, 0022, 0372, 0035, 0172, 0032, 0365, 0036, 0376, 0275, 0154, 0225, 0225, 0162, 0022, 0250, 
  0310, 0222, 0154, 0335, 0044, 0064, 0071, 0027, 0054, 0155, 0227, 0364, 0360, 0200, 0251, 0067, 
  0056, 0033, 0155, 0321, 0042, 0233, 0113, 0300, 0212, 0116, 0242, 0353, 0346, 0137, 0107, 0157, 
  0237, 0177, 0265, 0307, 0041, 0164, 0172, 0025, 0057, 0016, 0242, 0325, 0124, 0164, 0301, 0372, 
  0024, 0071, 0324, 0036, 0247, 0352, 0307, 0233, 0160, 0225, 0075, 0074, 0123, 0071, 0313, 0362, 
  0324, 0315, 0112, 0171, 0262, 0311, 0064, 0372, 0065, 0177, 0214, 0174, 0362, 0120, 0043, 0375, 
  0331, 0326, 0344, 0231, 0307, 0146, 0216, 0322, 0013, 0002, 0214, 0100, 0164, 0265, 0025, 0267, 
  0252, 0063, 0317, 0233, 0027, 0037, 0252, 0217, 0117, 0312, 0027, 0176, 0254, 0223, 0060, 0077, 
  0175, 0320, 0076, 0373, 0351, 0116, 0062, 0045, 0255, 0162, 0024, 0177, 0366, 0250, 0352, 0046, 
  0017, 0342, 0147, 0372, 0304, 0342, 0227, 0024, 0374, 0143, 0263, 0207, 0367, 0163, 0234, 0326, 
  0215, 0066, 0253, 0057, 0176, 0114, 0112, 0347, 0237, 0370, 0344, 0057, 0306, 0334, 0160, 0205, 
  0040, 0351, 0314, 0213, 0251, 0267, 0074, 0345, 0137, 0060, 0143, 0130, 0345, 0020, 0345, 0353, 
  0066, 0256, 0300, 0043, 0033, 0274, 0136, 0277, 0151, 0103, 0127, 0006, 0026, 0266, 0335, 0310, 
  0023, 0322, 0144, 0017, 0370, 0152, 0211, 0353, 0352, 0361, 0064, 0327, 0074, 0122, 0174, 0016, 
  0051, 0134, 0352, 0331, 0324, 0241, 0366, 0151, 0230, 0254, 0012, 0205, 0146, 0166, 0121, 0162, 
  0217, 0141, 0120, 0310, 0245, 0323, 0217, 0103, 0244, 0207, 0034, 0203, 0213, 0115, 0340, 0342, 
  0150, 0367, 0161, 0161, 0041, 0010, 0011, 0014, 0062, 0014, 0062, 0336, 0026, 0031, 0307, 0273, 
  0217, 0214, 0123, 0226, 0137, 0060, 0003, 0014, 0003, 0214, 0315, 0000, 0343, 0353, 0356, 0003, 
  0343, 0201, 0060, 0306, 0107, 0006, 0032, 0006, 0032, 0157, 0013, 0215, 0277, 0355, 0076, 0064, 
  0356, 0006, 0042, 0144, 0146, 0326, 0060, 0320, 0170, 0143, 0150, 0374, 0175, 0013, 0025, 0320, 
  0227, 0033, 0371, 0352, 0237, 0373, 0127, 0364, 0134, 0136, 0351, 0064, 0345, 0315, 0155, 0224, 
  0067, 0327, 0252, 0151, 0326, 0364, 0173, 0171, 0115, 0105, 0363, 0055, 0053, 0232, 0246, 0214, 
  0271, 0073, 0213, 0122, 0336, 0065, 0331, 0305, 0202, 0354, 0142, 0113, 0051, 0205, 0051, 0130, 
  0356, 0014, 0002, 0316, 0260, 0340, 0314, 0140, 0300, 0140, 0340, 0023, 0227, 0046, 0177, 0340, 
  0041, 0065, 0217, 0263, 0014, 0006, 0076, 0163, 0025, 0262, 0076, 0064, 0165, 0026, 0203, 0200, 
  0317, 0134, 0154, 0274, 0306, 0214, 0055, 0370, 0060, 0205, 0101, 0201, 0101, 0301, 0136, 0325, 
  0025, 0047, 0233, 0210, 0261, 0327, 0326, 0136, 0236, 0354, 0040, 0326, 0377, 0166, 0046, 0337, 
  0154, 0051, 0027, 0154, 0246, 0337, 0336, 0274, 0240, 0315, 0045, 0334, 0217, 0137, 0231, 0032, 
  0267, 0112, 0316, 0127, 0114, 0275, 0364, 0171, 0356, 0264, 0045, 0366, 0354, 0250, 0157, 0253, 
  0372, 0062, 0304, 0222, 0212, 0344, 0304, 0127, 0362, 0274, 0065, 0064, 0372, 0034, 0032, 0135, 
  0341, 0265, 0241, 0253, 0351, 0365, 0371, 0215, 0270, 0013, 0224, 0064, 0373, 0156, 0331, 0031, 
  0105, 0275, 0172, 0241, 0356, 0122, 0145, 0365, 0063, 0336, 0244, 0266, 0077, 0012, 0213, 0077, 
  0322, 0261, 0100, 0133, 0051, 0132, 0232, 0376, 0260, 0307, 0122, 0025, 0245, 0236, 0355, 0331, 
  0115, 0375, 0114, 0175, 0020, 0140, 0106, 0336, 0351, 0357, 0037, 0254, 0050, 0357, 0353, 0315, 
  0336, 0152, 0254, 0133, 0214, 0322, 0076, 0026, 0360, 0276, 0272, 0251, 0224, 0247, 0076, 0112, 
  0367, 0177, 0004, 0073, 0000, 0113, 0000, 0050, 0165, 0165, 0141, 0171, 0051, 0151, 0155, 0146, 
  0162, 0145, 0145, 0144, 0157, 0155, 0057, 0000, 0000, 0000, 0000, 0000, 0057, 0000, 0000, 0000, 
  0002, 0000, 0000, 0000, 0144, 0145, 0155, 0157, 0057, 0000, 0000, 0000, 0004, 0000, 0000, 0000, 
  0164, 0141, 0154, 0153, 0141, 0164, 0165, 0057, 0001, 0000, 0000, 0000
} };
#else /* _MSC_VER */
static const SECTION union { const guint8 data[2717]; const double alignment; void * const ptr;}  talkatudemo_resource_data = {
  "\107\126\141\162\151\141\156\164\000\000\000\000\000\000\000\000"
  "\030\000\000\000\034\001\000\000\000\000\000\050\011\000\000\000"
  "\000\000\000\000\000\000\000\000\004\000\000\000\006\000\000\000"
  "\006\000\000\000\006\000\000\000\010\000\000\000\010\000\000\000"
  "\011\000\000\000\006\074\270\052\005\000\000\000\034\001\000\000"
  "\005\000\114\000\044\001\000\000\050\001\000\000\320\061\002\041"
  "\010\000\000\000\050\001\000\000\010\000\114\000\060\001\000\000"
  "\064\001\000\000\113\120\220\013\006\000\000\000\064\001\000\000"
  "\004\000\114\000\070\001\000\000\074\001\000\000\075\237\024\265"
  "\001\000\000\000\074\001\000\000\003\000\114\000\100\001\000\000"
  "\104\001\000\000\042\042\032\100\007\000\000\000\104\001\000\000"
  "\007\000\166\000\120\001\000\000\155\012\000\000\122\134\116\150"
  "\002\000\000\000\155\012\000\000\012\000\114\000\170\012\000\000"
  "\174\012\000\000\324\265\002\000\377\377\377\377\174\012\000\000"
  "\001\000\114\000\200\012\000\000\204\012\000\000\061\200\145\173"
  "\003\000\000\000\204\012\000\000\005\000\114\000\214\012\000\000"
  "\220\012\000\000\153\354\170\204\000\000\000\000\220\012\000\000"
  "\010\000\114\000\230\012\000\000\234\012\000\000\153\145\145\160"
  "\057\000\000\000\010\000\000\000\164\141\154\153\141\164\165\057"
  "\003\000\000\000\157\162\147\057\005\000\000\000\165\151\057\000"
  "\007\000\000\000\144\145\155\157\056\165\151\000\000\000\000\000"
  "\276\161\000\000\001\000\000\000\170\332\355\135\133\163\342\072"
  "\022\176\347\127\150\375\164\116\355\070\044\231\275\114\355\020"
  "\316\222\014\111\250\311\255\200\324\124\236\050\141\013\133\033"
  "\141\371\110\002\302\326\376\370\155\331\046\201\140\203\315\045"
  "\201\104\057\251\140\267\244\276\175\255\126\133\262\053\177\074"
  "\365\031\032\022\041\051\017\116\254\243\203\103\013\221\300\341"
  "\056\015\274\023\353\276\175\156\177\263\376\250\226\052\177\261"
  "\155\164\101\002\042\260\042\056\032\121\345\043\217\141\227\240"
  "\257\007\137\277\035\034\243\122\251\215\331\043\126\003\004\164"
  "\355\237\100\341\172\104\111\324\343\002\071\076\126\010\207\041"
  "\243\016\126\060\214\054\235\361\160\054\250\347\053\364\333\331"
  "\357\350\370\360\350\237\366\361\341\361\041\272\300\142\214\176"
  "\012\334\007\122\037\125\074\101\373\377\026\004\207\104\214\270"
  "\140\356\201\303\373\125\030\311\247\022\205\202\173\100\210\340"
  "\337\236\040\004\111\336\123\043\054\310\167\064\346\003\344\340"
  "\000\011\342\122\251\004\355\016\024\101\024\070\010\334\062\160"
  "\323\007\331\172\343\022\134\030\004\056\021\110\371\004\051\042"
  "\372\022\361\136\364\343\342\346\076\021\225\241\273\101\027\130"
  "\101\127\324\041\201\044\010\303\270\372\212\364\101\011\335\161"
  "\111\223\237\353\321\133\311\350\350\234\103\257\221\224\337\021"
  "\001\055\301\000\211\162\321\361\144\200\244\267\057\210\213\322"
  "\157\240\032\140\130\040\036\352\106\277\003\227\143\304\100\311"
  "\317\355\016\346\005\176\221\313\105\064\210\372\364\171\010\142"
  "\150\105\203\140\043\312\030\352\022\064\220\244\067\140\137\112"
  "\100\211\176\065\332\227\267\367\155\124\273\171\100\277\152\315"
  "\146\355\246\375\360\075\062\044\207\273\144\110\342\176\150\037"
  "\314\244\055\214\205\300\201\032\003\317\245\353\172\363\354\022"
  "\350\153\247\215\253\106\373\001\330\106\347\215\366\115\275\325"
  "\102\347\267\115\124\103\167\265\146\273\161\166\177\125\153\242"
  "\273\373\346\335\155\253\176\200\120\213\150\206\110\151\201\072"
  "\173\221\071\100\153\056\121\230\062\011\242\076\200\361\044\260"
  "\304\134\344\343\041\001\043\072\204\016\201\041\214\034\160\232"
  "\345\066\052\141\306\003\057\366\120\245\365\306\150\127\200\133"
  "\175\107\264\207\002\256\276\040\011\214\125\174\245\102\371\257"
  "\162\171\064\032\035\170\301\340\200\013\257\314\342\036\144\271"
  "\012\214\330\066\170\075\015\300\020\075\354\220\152\011\241\212"
  "\040\177\016\250\040\121\227\047\126\342\357\326\013\170\016\001"
  "\074\345\024\112\117\075\376\165\212\354\353\301\321\267\204\116"
  "\243\352\171\014\073\031\337\126\143\260\245\027\262\341\061\322"
  "\134\314\321\005\270\117\320\063\334\322\050\134\042\035\101\043"
  "\217\132\216\306\364\056\234\147\210\346\001\345\244\017\105\300"
  "\177\300\173\221\303\260\224\317\112\372\101\372\374\027\015\134"
  "\076\262\120\010\060\011\324\211\165\241\036\223\113\272\045\264"
  "\005\007\207\076\301\345\264\174\047\026\100\330\356\161\147\040"
  "\255\352\071\146\222\124\312\023\202\164\372\056\027\000\147\033"
  "\104\125\276\125\075\072\236\243\227\324\013\300\135\142\152\227"
  "\060\242\210\255\375\136\131\340\152\201\313\210\070\261\124\314"
  "\157\307\005\206\073\243\210\275\216\303\270\044\156\307\351\132"
  "\110\216\100\161\304\075\261\002\036\133\020\272\165\174\312\334"
  "\370\177\370\305\273\377\041\216\232\210\017\062\336\341\200\270"
  "\326\344\376\074\333\103\052\151\227\021\253\332\026\203\071\031"
  "\227\350\045\157\023\056\050\210\031\131\333\252\202\037\052\060"
  "\075\313\323\020\224\111\354\130\071\013\106\233\321\100\212\026"
  "\022\047\150\071\202\063\106\334\031\253\257\254\226\025\125\223"
  "\326\114\372\030\030\212\140\147\125\151\220\331\154\116\316\154"
  "\131\057\041\102\163\061\266\020\005\147\361\223\037\257\233\316"
  "\163\242\377\132\325\204\076\213\217\225\325\265\206\312\322\232"
  "\106\330\201\166\027\077\176\166\356\156\033\067\355\172\263\163"
  "\175\333\156\334\336\164\256\153\255\237\350\177\050\345\316\045"
  "\374\232\276\175\172\337\156\303\345\273\046\314\046\051\327\233"
  "\365\253\172\255\125\217\356\024\341\315\047\117\041\270\255\055"
  "\211\052\056\330\060\177\343\112\071\066\377\053\117\051\317\103"
  "\042\205\260\022\142\347\021\362\254\305\336\011\323\010\375\057"
  "\051\352\321\202\006\217\031\201\163\302\320\334\350\163\154\057"
  "\105\066\304\267\123\376\264\145\050\057\020\142\215\120\227\033"
  "\321\040\143\233\163\326\305\302\172\033\034\056\224\067\233\355"
  "\164\326\233\330\245\134\363\177\072\120\012\164\022\005\044\305"
  "\075\217\221\016\314\324\064\260\322\372\131\103\260\165\205\113"
  "\153\257\200\177\105\103\133\221\047\230\254\025\044\246\022\222"
  "\014\014\374\234\130\143\002\275\336\353\334\174\222\022\235\016"
  "\172\075\042\212\216\301\160\227\260\324\316\357\264\232\212\166"
  "\007\251\267\035\255\057\030\015\126\324\032\165\170\140\307\063"
  "\202\026\334\176\262\075\235\361\122\247\150\107\330\121\220\102"
  "\347\144\142\046\113\212\035\305\135\222\040\165\043\205\167\040"
  "\261\014\274\044\121\212\375\060\065\377\113\311\241\362\304\324"
  "\105\041\063\163\206\212\342\370\212\116\347\363\076\327\012\347"
  "\171\346\306\324\170\272\140\072\330\054\220\107\076\327\320\374"
  "\150\100\376\245\305\332\064\232\243\116\337\027\315\130\072\224"
  "\026\155\357\011\076\010\041\022\114\205\156\203\345\017\210\145"
  "\137\365\331\307\203\362\145\373\372\152\323\110\326\175\356\300"
  "\264\254\015\146\300\154\300\234\006\346\076\026\217\240\333\017"
  "\230\144\137\047\222\155\032\324\223\176\167\051\337\266\047\125"
  "\115\003\363\317\013\363\026\011\261\300\212\013\015\365\006\170"
  "\304\356\141\332\030\056\253\170\223\204\346\217\020\205\033\201"
  "\004\162\244\223\037\324\023\274\017\041\271\107\213\257\150\262"
  "\043\360\324\000\373\222\135\315\304\114\207\121\347\161\151\314"
  "\244\221\224\121\266\155\002\246\301\135\156\334\115\362\223\255"
  "\142\157\027\223\240\055\202\160\222\045\033\040\232\314\305\030"
  "\156\072\202\352\354\377\365\322\022\017\224\317\005\044\375\373"
  "\022\132\263\303\135\055\022\345\175\243\234\034\113\200\204\015"
  "\235\012\271\215\305\131\142\256\204\170\046\304\045\267\102\036"
  "\362\041\021\046\276\155\003\046\232\343\216\303\331\007\101\314"
  "\231\226\344\175\001\023\051\323\226\204\201\025\154\071\356\167"
  "\071\053\232\035\024\102\316\224\005\027\200\150\212\312\340\311"
  "\344\013\046\020\276\056\105\253\314\075\054\373\267\026\213\005"
  "\105\211\110\050\142\226\062\252\306\233\013\264\311\226\237\255"
  "\207\132\343\311\205\075\131\222\300\335\243\311\074\257\067\153"
  "\261\120\054\326\126\074\272\245\373\217\325\151\274\172\367\274"
  "\232\270\124\315\154\024\337\173\207\036\371\044\072\377\242\317"
  "\153\364\211\224\330\043\372\360\112\054\350\346\374\272\276\122"
  "\177\306\245\063\134\072\113\316\154\031\127\226\357\165\303\036"
  "\145\254\370\316\331\220\113\032\157\367\075\134\264\117\073\175"
  "\257\165\232\016\212\354\363\327\316\307\105\014\144\022\377\277"
  "\234\343\102\173\224\067\272\313\330\154\331\177\061\160\174\326"
  "\011\026\245\166\364\063\216\153\131\031\162\272\365\333\205\162"
  "\352\135\122\376\332\006\130\333\010\233\214\250\161\340\310\265"
  "\156\134\165\206\237\363\026\032\204\003\125\300\127\032\231\364"
  "\357\157\211\224\303\164\321\226\022\253\232\154\055\131\141\353"
  "\012\120\006\366\100\260\045\105\236\041\045\243\216\246\355\000"
  "\355\332\017\200\136\063\241\163\152\073\311\075\362\060\022\055"
  "\055\022\372\035\172\032\365\036\376\275\154\225\225\162\022\250"
  "\310\222\154\335\044\064\071\027\054\155\227\364\360\200\251\067"
  "\056\033\155\321\042\233\113\300\212\116\242\353\346\137\107\157"
  "\237\177\265\307\041\164\172\025\057\016\242\325\124\164\301\372"
  "\024\071\324\036\247\352\307\233\160\225\075\074\123\071\313\362"
  "\324\315\112\171\262\311\064\372\065\177\214\174\362\120\043\375"
  "\331\326\344\231\307\146\216\322\013\002\214\100\164\265\025\267"
  "\252\063\317\233\027\037\252\217\117\312\027\176\254\223\060\077"
  "\175\320\076\373\351\116\062\045\255\162\024\177\366\250\352\046"
  "\017\342\147\372\304\342\227\024\374\143\263\207\367\163\234\326"
  "\215\066\253\057\176\114\112\347\237\370\344\057\306\334\160\205"
  "\040\351\314\213\251\267\074\345\137\060\143\130\345\020\345\353"
  "\066\256\300\043\033\274\136\277\151\103\127\006\026\266\335\310"
  "\023\322\144\017\370\152\211\353\352\361\064\327\074\122\174\016"
  "\051\134\352\331\324\241\366\151\230\254\012\205\146\166\121\162"
  "\217\141\120\310\245\323\217\103\244\207\034\203\213\115\340\342"
  "\150\367\161\161\041\010\011\014\062\014\062\336\026\031\307\273"
  "\217\214\123\226\137\060\003\014\003\214\315\000\343\353\356\003"
  "\343\201\060\306\107\006\032\006\032\157\013\215\277\355\076\064"
  "\356\006\042\144\146\326\060\320\170\143\150\374\175\013\025\320"
  "\227\033\371\352\237\373\127\364\134\136\351\064\345\315\155\224"
  "\067\327\252\151\326\364\173\171\115\105\363\055\053\232\246\214"
  "\271\073\213\122\336\065\331\305\202\354\142\113\051\205\051\130"
  "\356\014\002\316\260\340\314\140\300\140\340\023\227\046\177\340"
  "\041\065\217\263\014\006\076\163\025\262\076\064\165\026\203\200"
  "\317\134\154\274\306\214\055\370\060\205\101\201\101\301\136\325"
  "\025\047\233\210\261\327\326\136\236\354\040\326\377\166\046\337"
  "\154\051\027\154\246\337\336\274\240\315\045\334\217\137\231\032"
  "\267\112\316\127\114\275\364\171\356\264\045\366\354\250\157\253"
  "\372\062\304\222\212\344\304\127\362\274\065\064\372\034\032\135"
  "\341\265\241\253\351\365\371\215\270\013\224\064\373\156\331\031"
  "\105\275\172\241\356\122\145\365\063\336\244\266\077\012\213\077"
  "\322\261\100\133\051\132\232\376\260\307\122\025\245\236\355\331"
  "\115\375\114\175\020\140\106\336\351\357\037\254\050\357\353\315"
  "\336\152\254\133\214\322\076\026\360\276\272\251\224\247\076\112"
  "\367\177\004\073\000\113\000\050\165\165\141\171\051\151\155\146"
  "\162\145\145\144\157\155\057\000\000\000\000\000\057\000\000\000"
  "\002\000\000\000\144\145\155\157\057\000\000\000\004\000\000\000"
  "\164\141\154\153\141\164\165\057\001\000\000\000" };
#endif /* !_MSC_VER */

static GStaticResource static_resource = { talkatudemo_resource_data.data, sizeof (talkatudemo_resource_data.data) - 1 /* nul terminator */, NULL, NULL, NULL };

G_GNUC_INTERNAL
GResource *talkatudemo_get_resource (void);
GResource *talkatudemo_get_resource (void)
{
  return g_static_resource_get_resource (&static_resource);
}
/*
  If G_HAS_CONSTRUCTORS is true then the compiler support *both* constructors and
  destructors, in a usable way, including e.g. on library unload. If not you're on
  your own.

  Some compilers need #pragma to handle this, which does not work with macros,
  so the way you need to use this is (for constructors):

  #ifdef G_DEFINE_CONSTRUCTOR_NEEDS_PRAGMA
  #pragma G_DEFINE_CONSTRUCTOR_PRAGMA_ARGS(my_constructor)
  #endif
  G_DEFINE_CONSTRUCTOR(my_constructor)
  static void my_constructor(void) {
   ...
  }

*/

#ifndef __GTK_DOC_IGNORE__

#if  __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 7)

#define G_HAS_CONSTRUCTORS 1

#define G_DEFINE_CONSTRUCTOR(_func) static void __attribute__((constructor)) _func (void);
#define G_DEFINE_DESTRUCTOR(_func) static void __attribute__((destructor)) _func (void);

#elif defined (_MSC_VER) && (_MSC_VER >= 1500)
/* Visual studio 2008 and later has _Pragma */

#include <stdlib.h>

#define G_HAS_CONSTRUCTORS 1

/* We do some weird things to avoid the constructors being optimized
 * away on VS2015 if WholeProgramOptimization is enabled. First we
 * make a reference to the array from the wrapper to make sure its
 * references. Then we use a pragma to make sure the wrapper function
 * symbol is always included at the link stage. Also, the symbols
 * need to be extern (but not dllexport), even though they are not
 * really used from another object file.
 */

/* We need to account for differences between the mangling of symbols
 * for x86 and x64/ARM/ARM64 programs, as symbols on x86 are prefixed
 * with an underscore but symbols on x64/ARM/ARM64 are not.
 */
#ifdef _M_IX86
#define G_MSVC_SYMBOL_PREFIX "_"
#else
#define G_MSVC_SYMBOL_PREFIX ""
#endif

#define G_DEFINE_CONSTRUCTOR(_func) G_MSVC_CTOR (_func, G_MSVC_SYMBOL_PREFIX)
#define G_DEFINE_DESTRUCTOR(_func) G_MSVC_DTOR (_func, G_MSVC_SYMBOL_PREFIX)

#define G_MSVC_CTOR(_func,_sym_prefix) \
  static void _func(void); \
  extern int (* _array ## _func)(void);              \
  int _func ## _wrapper(void) { _func(); g_slist_find (NULL,  _array ## _func); return 0; } \
  __pragma(comment(linker,"/include:" _sym_prefix # _func "_wrapper")) \
  __pragma(section(".CRT$XCU",read)) \
  __declspec(allocate(".CRT$XCU")) int (* _array ## _func)(void) = _func ## _wrapper;

#define G_MSVC_DTOR(_func,_sym_prefix) \
  static void _func(void); \
  extern int (* _array ## _func)(void);              \
  int _func ## _constructor(void) { atexit (_func); g_slist_find (NULL,  _array ## _func); return 0; } \
   __pragma(comment(linker,"/include:" _sym_prefix # _func "_constructor")) \
  __pragma(section(".CRT$XCU",read)) \
  __declspec(allocate(".CRT$XCU")) int (* _array ## _func)(void) = _func ## _constructor;

#elif defined (_MSC_VER)

#define G_HAS_CONSTRUCTORS 1

/* Pre Visual studio 2008 must use #pragma section */
#define G_DEFINE_CONSTRUCTOR_NEEDS_PRAGMA 1
#define G_DEFINE_DESTRUCTOR_NEEDS_PRAGMA 1

#define G_DEFINE_CONSTRUCTOR_PRAGMA_ARGS(_func) \
  section(".CRT$XCU",read)
#define G_DEFINE_CONSTRUCTOR(_func) \
  static void _func(void); \
  static int _func ## _wrapper(void) { _func(); return 0; } \
  __declspec(allocate(".CRT$XCU")) static int (*p)(void) = _func ## _wrapper;

#define G_DEFINE_DESTRUCTOR_PRAGMA_ARGS(_func) \
  section(".CRT$XCU",read)
#define G_DEFINE_DESTRUCTOR(_func) \
  static void _func(void); \
  static int _func ## _constructor(void) { atexit (_func); return 0; } \
  __declspec(allocate(".CRT$XCU")) static int (* _array ## _func)(void) = _func ## _constructor;

#elif defined(__SUNPRO_C)

/* This is not tested, but i believe it should work, based on:
 * http://opensource.apple.com/source/OpenSSL098/OpenSSL098-35/src/fips/fips_premain.c
 */

#define G_HAS_CONSTRUCTORS 1

#define G_DEFINE_CONSTRUCTOR_NEEDS_PRAGMA 1
#define G_DEFINE_DESTRUCTOR_NEEDS_PRAGMA 1

#define G_DEFINE_CONSTRUCTOR_PRAGMA_ARGS(_func) \
  init(_func)
#define G_DEFINE_CONSTRUCTOR(_func) \
  static void _func(void);

#define G_DEFINE_DESTRUCTOR_PRAGMA_ARGS(_func) \
  fini(_func)
#define G_DEFINE_DESTRUCTOR(_func) \
  static void _func(void);

#else

/* constructors not supported for this compiler */

#endif

#endif /* __GTK_DOC_IGNORE__ */

#ifdef G_HAS_CONSTRUCTORS

#ifdef G_DEFINE_CONSTRUCTOR_NEEDS_PRAGMA
#pragma G_DEFINE_CONSTRUCTOR_PRAGMA_ARGS(resource_constructor)
#endif
G_DEFINE_CONSTRUCTOR(resource_constructor)
#ifdef G_DEFINE_DESTRUCTOR_NEEDS_PRAGMA
#pragma G_DEFINE_DESTRUCTOR_PRAGMA_ARGS(resource_destructor)
#endif
G_DEFINE_DESTRUCTOR(resource_destructor)

#else
#warning "Constructor not supported on this compiler, linking in resources will not work"
#endif

static void resource_constructor (void)
{
  g_static_resource_init (&static_resource);
}

static void resource_destructor (void)
{
  g_static_resource_fini (&static_resource);
}
