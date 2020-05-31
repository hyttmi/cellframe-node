#!/bin/bash

echo "Deploying to $PACKAGE_PATH"
echo $wd

CELLFRAME_REPO_CREDS="admin@debian.pub.demlabs.net"
CELLFRAME_REPO_KEY="~/.ssh/demlabs_publish"
CELLFRAME_REPO_PATH="~/web/debian.pub.demlabs.net/public_html"
CELLFRAME_FILESERVER_CREDS="admin@pub.cellframe.net"
CELLFRAME_FILESERVER_PATH="~/web/pub.cellframe.net/public_html/linux"
pwd

cd packages
PKGFILES=$(ls . | grep .deb)
#cd ..

#echo "We have $DISTR_CODENAME there"
#echo "On path $REPO_DIR_SRC we have debian files."
[[ $ONBUILDSERVER == 0 ]] && scp -i $CELLFRAME_REPO_KEY ../prod_build/linux/debian/scripts/publish_remote/reprepro.sh "$CELLFRAME_REPO_CREDS:~/tmp/"
for pkgfile in $PKGFILES; do
	pkgname=$(echo $pkgfile | sed 's/.deb$//')
	pkgname_public=$(echo $pkgname | cut -d '-' -f1-4,7-) #cutting away Debian-9.12
	pkgname_weblink="$(echo $pkgname | cut -d '-' -f2,8 )-latest" #leaving only necessary entries
	mv $pkgfile $wd/$PACKAGE_PATH/$pkgname$MOD.deb || { echo "[ERR] Something went wrong in publishing the package. Now aborting."; exit -4; }
	CODENAME=$(echo $pkgname | rev | cut -d '-' -f1 | rev)
	cp -r ../prod_build/general/essentials/weblink-latest ../prod_build/general/essentials/$pkgname_weblink
	sed -i "/document/s/cellframe.*deb/$pkgname_public$MOD.deb/" ../prod_build/general/essentials/$pkgname_weblink/index.php
	if [[ $ONBUILDSERVER == 0 ]]; then
	#REPREPRO and file_cellframe_services
		scp -i $CELLFRAME_REPO_KEY $wd/$PACKAGE_PATH/$pkgname$MOD.deb "$CELLFRAME_REPO_CREDS:~/tmp/apt/"
		ssh -i $CELLFRAME_REPO_KEY "$CELLFRAME_REPO_CREDS" "chmod +x ~/tmp/reprepro.sh && ~/tmp/reprepro.sh main $CODENAME ~/tmp/apt/$pkgname_public$MOD.deb $CELLFRAME_REPO_PATH"
		scp -i $CELLFRAME_REPO_KEY $wd/$PACKAGE_PATH/$pkgname$MOD.deb "$CELLFRAME_FILESERVER_CREDS:$CELLFRAME_FILESERVER_PATH/$pkgname_public$MOD.deb"
		scp -r -i $CELLFRAME_REPO_KEY ../prod_build/general/essentials/$pkgname_weblink "$CELLFRAME_FILESERVER_CREDS:$CELLFRAME_FILESERVER_PATH/"
#		ssh -i $CELLFRAME_REPO_KEY "$CELLFRAME_FILESERVER_CREDS" "ln -sf $CELLFRAME_FILESERVER_PATH/$pkgname$MOD.deb $CELLFRAME_FILESERVER_PATH/$pkgname$MOD-latest.deb"
	fi
	rm -r ../prod_build/general/essentials/$pkgname_weblink
done
[[ $ONBUILDSERVER == 0 ]] && ssh -i $CELLFRAME_REPO_KEY "$CELLFRAME_REPO_CREDS" "rm -v ~/tmp/reprepro.sh"

#	export -n "UPDVER"
cd ..
exit 0
#symlink name-actual to the latest version.
#build/deb/versions - for all files
#build/deb/${PROJECT}-latest - for symlinks.
