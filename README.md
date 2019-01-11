## LAMMPS
**Adapt the code of [lammps-induced-dipole-polarization-pair-style][polarization] to the latest version of [lammps]**

## Build & Run
```shell
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DPKG_MOLECULE=ON -DPKG_KSPACE=ON -DPKG_RIGID=ON ../cmake/
# cmake -DCMAKE_BUILD_TYPE=Debug -DPKG_MOLECULE=ON -DPKG_KSPACE=ON -DPKG_RIGID=ON ../cmake/
make
```

## Acknowledgements
Special thanks to the contributors:
 - [aurix](https://github.com/aurix) - [lammps-induced-dipole-polarization-pair-style][polarization]
 - [lammps](http://lammps.sandia.gov/) - [lammps][lammps]


[lammps]: https://github.com/lammps/lammps
[polarization]: https://github.com/aurix/lammps-induced-dipole-polarization-pair-style

