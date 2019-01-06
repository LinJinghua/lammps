## LAMMPS
**Adapt the code of [lammps-induced-dipole-polarization-pair-style][polarization] to the latest version of [lammps]**

## Build & Run
```shell
mkdir -p build && cd build
cmake ../cmake/ -DPKG_MOLECULE=ON -DPKG_KSPACE=ON -DPKG_RIGID=ON
make -j8
```

## Acknowledgements
Special thanks to the contributors:
 - [aurix](https://github.com/aurix) - [lammps-induced-dipole-polarization-pair-style][polarization]
 - [lammps](http://lammps.sandia.gov/) - [lammps][lammps]


[lammps]: https://github.com/lammps/lammps
[polarization]: https://github.com/aurix/lammps-induced-dipole-polarization-pair-style

