module parameters

use nrtype

real(dp), parameter :: L=6.9006555934235427330_dp ! Dimensionless length used here
integer(i4b), parameter :: M=16, d=128
real(dp), parameter :: h=0.025_dp, R=1.0_dp, ti=0.0_dp,tf=2000.0_dp
integer(i4b),parameter :: Nplt=1000
character(len=*), parameter :: frm_t='(1000F12.4)', frm_u='(128F19.12)'

end module parameters
