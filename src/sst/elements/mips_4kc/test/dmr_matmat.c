#define SIDE 12

typedef unsigned int uint32_t;

volatile uint32_t firstM[SIDE][SIDE] =  {
    { 2067261, 2017463455, 1138961335, 1688969677, 1188541828, 366694711, 448260522, 1236480519, 1184067167, 343865911, 753283272, 953755623},
    { 168636592, 669331060, 360607371, 2081904114, 604985272, 1155500400, 1749219598, 1927577930, 828462531, 1905310403, 293742895, 1484062225},
    { 1166783511, 598842305, 528829321, 407146696, 1807404079, 1787987373, 584824989, 1082141766, 766608236, 1580063467, 1234808992, 1260514187},
    { 174078107, 341298340, 1590285344, 1430770104, 1764217798, 2043818720, 1705955009, 1193174685, 2004287539, 1466667008, 375542294, 1518207912},
    { 1473033718, 270635523, 189813921, 702488488, 1948964205, 1494443365, 1583405107, 1093871208, 459160639, 1839239933, 1398882173, 1952762775},
    { 411911863, 792316062, 656665644, 776031494, 537293504, 214429343, 1054870440, 977552932, 1005338018, 480938949, 1252753986, 871824778},
    { 2032651128, 953121721, 24799148, 162682677, 1077583980, 574902609, 1257556842, 989958143, 1040533719, 1901903590, 1052583333, 986085416},
    { 1161517966, 292464446, 724467532, 1590895636, 270702593, 704030517, 2129165181, 1074137403, 327415621, 1266717307, 1643787219, 2072893833},
    { 1821989174, 2080384351, 39076467, 80808579, 1599128664, 899573327, 1516486828, 221977191, 1551284523, 2010359729, 267909537, 1974481844},
    { 1049260978, 496187726, 2108458329, 2123209153, 1612643052, 267415448, 1170880860, 1081404674, 1944850860, 843391280, 1096148239, 1793335153},
    { 912694764, 1087505286, 1963747981, 1914156214, 1181238808, 157174812, 262632919, 555861840, 2033217268, 530882138, 423559248, 1351846990},
    { 2088636096, 652443820, 2084085828, 248333292, 2044385191, 293742377, 1190771239, 2067076653, 1842539167, 1276318261, 40098582, 1683535652}};
volatile uint32_t secondM[SIDE][SIDE] =  {
    { 384717275, 888985702, 2001411634, 1074515293, 2077102449, 1907424534, 541959578, 328830814, 2033402667, 475872100, 1015853439, 952814553},
    { 1744271351, 927782434, 529232563, 1611383427, 1799881606, 800602979, 82656156, 2011454515, 1833275016, 1423282804, 2019415459, 1758739317},
    { 1457288620, 1634250293, 1747066761, 1031620330, 884168938, 965105540, 120937804, 517654719, 1630224099, 343911067, 152763936, 535763254},
    { 858017135, 272379243, 344306046, 1578742469, 901816857, 1460293275, 931665166, 484635109, 632181131, 1455103190, 284896725, 119683330},
    { 1086215810, 200870715, 1189354452, 2006410257, 673325744, 140900243, 672279725, 85890889, 1204116002, 1225939013, 361714255, 91382324},
    { 1662887160, 2057442634, 661016775, 1093131427, 123186093, 436408135, 1729979095, 1482228574, 314733930, 12468535, 1106567514, 478120365},
    { 597651820, 1036241874, 187452918, 461069708, 1224356709, 859221610, 223789720, 1670696092, 1290877712, 2147035182, 1945277392, 1006282813},
    { 1006103332, 2007359586, 2041015481, 2011549102, 1328116205, 6004249, 1359187106, 1279795539, 1021238533, 1712386038, 1902154725, 499445950},
    { 1202724845, 1838530450, 1775668534, 940122349, 797613643, 844032009, 1258195600, 591554298, 1987503481, 1755747052, 1629864247, 51555017},
    { 1941031729, 748109581, 1231476356, 40472272, 300666177, 1915645012, 1601956559, 1014251357, 259813033, 1485172760, 1848728907, 650930826},
    { 185208027, 468022185, 36145924, 1918456638, 1741813188, 232179474, 992575048, 816080430, 1522832212, 1889023728, 1999474978, 115375670},
    { 939171610, 571781158, 1772228626, 1176912523, 243553137, 2010709433, 892107480, 1514349452, 841590029, 2014346391, 1774486163, 2086653939}};
uint32_t goldenM[SIDE][SIDE] =  {
    { 2640226171, 598129597, 922251052, 657648016, 4147680461, 67769528, 2569994484, 392767413, 262462445, 1932701235, 2929937441, 1988706293},
    { 3651713495, 20848311, 1230362613, 1084218532, 2677097841, 3830806106, 36399241, 555042828, 2612607528, 609266480, 315137959, 3568183298},
    { 2840850983, 1327440375, 3394690119, 2739758619, 2509077009, 3851443140, 1430774445, 3722092522, 572443716, 2438497912, 1990271603, 1978638375},
    { 944129346, 3774713964, 3842930319, 2539288798, 1064529258, 2125324918, 1171990334, 375471849, 1103929209, 1734380498, 1267718365, 2471109321},
    { 2902706102, 2825966244, 2879584319, 1926409887, 4077115487, 2281548301, 1670438387, 3508692164, 1330250724, 1410362778, 3825497199, 1706622835},
    { 4170829258, 991621573, 4002023233, 3833817460, 1091444805, 439737415, 3240163582, 3293780005, 3876016326, 3486477187, 2762031938, 2780187434},
    { 2814845918, 3839952174, 92663245, 3238754790, 3472687476, 293673585, 3366572101, 3119131777, 1668826585, 3604806091, 275863402, 299290297},
    { 55692481, 2444948687, 3446805322, 1916238784, 2856044662, 3514639845, 153157583, 4275170569, 2615822102, 3199205343, 2425093093, 3050444164},
    { 3332077795, 2965038042, 191581568, 2306946744, 2152273877, 345612211, 3449178948, 4074679733, 3807506417, 377088562, 3777490765, 1573190953},
    { 2795937518, 385927205, 1414008213, 238993100, 3633704104, 591724462, 1888277438, 3031428266, 3316654787, 1072591768, 2404391776, 3500804921},
    { 2072094098, 492109523, 367693181, 4204200028, 1124371275, 1511741052, 23526090, 3648151409, 571219049, 2639538891, 419714856, 1084626397},
    { 1898056024, 19393756, 1359536184, 2355018695, 826121043, 2915568978, 4100178223, 149975503, 1124386233, 3814409694, 2607260364, 3904570135}};

uint32_t resultM[SIDE][SIDE];

int main() {
  int i,j,k,n;
  int errors = 0;
  unsigned long sum[2] = {0,0};

  // do mat mat
  for ( i = 0 ; i < SIDE ; i++ ) {
    for ( j = 0 ; j < SIDE ; j++ ) {

      do {
	sum[0] = sum[1] = 0;
	for (k = 0 ; k < SIDE ; k++ ) {
	  for (n = 0 ; n < 2 ; n++ ) {
	    sum[n] = sum[n] + firstM[i][k]*secondM[k][j];
	  }
	}
      } while(sum[0] != sum[1]); 
      
      resultM[i][j] = sum[0];
    }
  }
  
  // compare
  for ( i = 0 ; i < SIDE ; i++ ) {
    for ( j = 0 ; j < SIDE ; j++ ) {
      if (resultM[i][j] != goldenM[i][j]) {
	errors++;
	asm volatile ("move $a0, %0\n"   /* Move 'b' into $a0 */
		      "li $v0, 1\n"      /* Set for 'PRINT_INT' syscall */
		      "syscall" : : "r" (resultM[i][j]));
	asm volatile ("move $a0, %0\n"   /* Move 'b' into $a0 */
		      "li $v0, 1\n"      /* Set for 'PRINT_INT' syscall */
		      "syscall" : : "r" (goldenM[i][j]));
      }
    }
  }

  asm volatile ("move $a0, %0\n"   /* Move 'b' into $a0 */
		"li $v0, 1\n"      /* Set for 'PRINT_INT' syscall */
		"syscall" : : "r" (errors));
  return 0;
}
