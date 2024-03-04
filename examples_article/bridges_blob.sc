<obstacles>
 <friction> 0
 <wall>
  <position> 10
  <axis> 0
  <dir> 1
 </wall>
 <wall>
  <position> 10
  <axis> 1
  <dir> 1
 </wall>
 <wall>
  <position> 290
  <axis> 1
  <dir> -1
 </wall>
 <rbox>
  <position> 50 300
  <normal> 0.5 1
  <size> 90 10
  <out>
 </rbox>
 <rbox>
  <position> 250 300
  <normal> -0.5 1
  <size> 90 10
  <out>
 </rbox>
</obstacles>
<particules>
 <mass> 0.2
 <radius> 5
# <coefs> 13 1 0 0 0 0 0 0 0 0 0 0 1 0 # flower
 <coefs> 15 1 0 0 0.3 0.4 0 0 0 0 0.5 0 0 0 0 0 #blob
# <coefs> 21 1.5 0 0 0 0.5 0 0.2 0 0 0 0 1 0 0 0 0 0 0 0 0 0.8 #other
 <random rotation>
 <cuboid> 
  <x> 50 250
  <y> 350 600
  <nb particules> 10000
 </cuboid>
</particules>