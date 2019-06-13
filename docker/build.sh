BASEDIR=$(dirname $(realpath "$0"))
cd $BASEDIR

docker build -f base.Dockerfile -t h3-pg:base ..
docker build -f build.Dockerfile -t h3-pg:build ..

cd ..
docker run --rm -v "$PWD":/tmp/h3-pg h3-pg:build