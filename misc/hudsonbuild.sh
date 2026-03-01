#!/bin/bash

PROJECT_ROOT=$(dirname $(dirname $(readlink -f $0)))

create_env() {
  envpath=$1
  python3 -m venv --system-site-packages $envpath || exit 127
}

init_env() {
  envpath=$1
  baltradcrypto=$2
  baltradutils=$3

  if [ -e $envpath ]; then
    \rm -fr "$envpath"
  fi
  create_env $envpath

  source $envpath/bin/activate
  if [ "$baltradcrypto" != "" ]; then
    $envpath/bin/pip3 install "$baltradcrypto"
  fi

  if [ "$baltradutils" != "" ]; then
    $envpath/bin/pip3 install "$baltradutils"
  fi
}

init_test_env() {
  envpath=$1
  baltradcrypto=$2
  baltradutils=$3

  init_env "$envpath" "$baltradcrypto" "$baltradutils"

  python3 -m pip install "sphinx >= 1.1" --trusted-host pypi.python.org
  python3 -m pip install "cherrypy >= 18.10.0" --trusted-host pypi.python.org
  python3 -m pip install "psycopg2 >= 2.8" --trusted-host pypi.python.org
  python3 -m pip install "werkzeug >= 2.0" --trusted-host pypi.python.org
  python3 -m pip install "pytest >= 7.4.3" --trusted-host pypi.python.org
  python3 -m pip install "mock>=0.7,<=4.0.3" --trusted-host pypi.python.org
}

install_python_package() {
  package_dir=$1
  cd $package_dir
  python3 -m pip install .
}

test_python_package() {
  package_dir=$1
  cd $package_dir
  python3 -m pip install -e .
  python3 -m pytest --junitxml=$package_dir/test-results.xml
}

test_java_client() {
  property_file="$PROJECT_ROOT/misc/hudsonbuild.properties"

  cd "$PROJECT_ROOT/client/java"
  ant hudson -propertyfile $property_file
}

install_java_client() {
  cd "$PROJECT_ROOT/client/java"
  ant install -Dprefix="$PREFIX/env"
}

run_tests() {
  test_python_package "$PROJECT_ROOT/common"

  export BDB_TEST_DB
  test_python_package "$PROJECT_ROOT/server"
    
  # Important that client api is tested after server since we need to start server for
  # integration tests
  install_python_package "$PROJECT_ROOT/common"
  install_python_package "$PROJECT_ROOT/server"

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

BALTRADCRYPTO=
BALTRADUTILS=
TEST_ENV=$PROJECT_ROOT

for arg in $*; do
  case $arg in
    --testenvroot=*)
      TEST_ENV=`echo $arg | sed 's/[-a-zA-Z0-9]*=//'`
      ;;
    --prefix=*)
      PREFIX=`echo $arg | sed 's/[-a-zA-Z0-9]*=//'`
      ;;
    --hlhdf-root=*)
      HLHDF_ROOT=`echo $arg | sed 's/[-a-zA-Z0-9]*=//'`
      ;;
    --test-db=*)
      BDB_TEST_DB=`echo $arg | sed 's/[-a-zA-Z0-9]*=//'`
      ;;
    --baltradcrypto=*)
      BALTRADCRYPTO=`echo $arg | sed 's/[-a-zA-Z0-9]*=//'`
      ;;
    --baltradutils=*)
      BALTRADUTILS=`echo $arg | sed 's/[-a-zA-Z0-9]*=//'`
      ;;
  esac
done

init_test_env "$TEST_ENV/test-env" "$BALTRADCRYPTO" "$BALTRADUTILS"

run_tests
deactivate

init_env "$PREFIX/env" "$BALTRADCRYPTO" "$BALTRADUTILS"
deploy


