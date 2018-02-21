#!/bin/bash

PROJECT_ROOT=$(dirname $(dirname $(readlink -f $0)))
HLHDF_ROOT=/home/hudson/installed/baltrad_3p/hlhdf
PREFIX=/home/hudson/installed/baltrad_3p/baltrad-db

create_env() {
  envpath=$1
  python $PROJECT_ROOT/misc/virtualenv/virtualenv.py \
    --system-site-packages \
    $envpath
}

init_env() {
  envpath=$1

  if [ -e $envpath ]; then
    \rm -fr "$envpath"
  fi
  create_env $envpath

  source $envpath/bin/activate

  cp $HLHDF_ROOT/hlhdf.pth $envpath/lib/python2.7/site-packages
  export LD_LIBRARY_PATH=$HLHDF_ROOT/lib:$LD_LIBRARY_PATH
}

init_test_env() {
  envpath=$1

  init_env $envpath

  $envpath/bin/pip install "nose >= 1.1" --trusted-host pypi.python.org
  $envpath/bin/pip install "sphinx >= 1.1" --trusted-host pypi.python.org
  $envpath/bin/pip install "mock >= 0.7" --trusted-host pypi.python.org
  $envpath/bin/pip install "cherrypy == 3.2.2" --trusted-host pypi.python.org
}

install_python_package() {
  package_dir=$1

  cd $package_dir
  python setup.py -q install
}

test_python_package() {
  package_dir=$1

  cd $package_dir
  python setup.py -q develop
  python setup.py nosetests \
    --first-package-wins \
    --with-xunit \
    --xunit-file=$package_dir/test-results.xml
}

test_java_client() {
  property_file="$PROJECT_ROOT/misc/hudsonbuild.properties"

  cd "$PROJECT_ROOT/client/java"
  ant hudson -propertyfile $property_file
}

install_java_client() {
  cd "$PROJECT_ROOT/client/java"
  ant install -Dprefix=$PREFIX
}

run_tests() {
  test_python_package "$PROJECT_ROOT/common"
  export BDB_TEST_DB
  test_python_package "$PROJECT_ROOT/server"
  # Important that client api is tested after server since we need to start server for
  # integration tests
  export BDB_PYCLIENT_ITEST_PROPERTYFILE="$PROJECT_ROOT/misc/hudsonbuild.properties"
  test_python_package "$PROJECT_ROOT/client/python"
  test_java_client
}

deploy() {
  install_python_package "$PROJECT_ROOT/common"
  install_python_package "$PROJECT_ROOT/server"
  install_python_package "$PROJECT_ROOT/client/python"
  install_java_client
}

for arg in $*; do
  case $arg in
    --prefix=*)
      PREFIX=`echo $arg | sed 's/[-a-zA-Z0-9]*=//'`
      ;;
    --hlhdf-root=*)
      HLHDF_ROOT=`echo $arg | sed 's/[-a-zA-Z0-9]*=//'`
      ;;
    --test-db=*)
      BDB_TEST_DB=`echo $arg | sed 's/[-a-zA-Z0-9]*=//'`
  esac
done

init_test_env "$PROJECT_ROOT/test-env/"
run_tests
deactivate

init_env "$PREFIX/env"
deploy


