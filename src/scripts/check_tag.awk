#-------------------------------------------------------------------------------
# Description :
# 	Exploitation des donnees CVS sur les versions et les tags
#
# Entrees :
# 	Ce script utilise les informations fournie par "cvs status -v"
#
# Utilisation :
# 	ou "cvs status -v | nawk -f THIS_AWK_FILE -v output=OUTPUT_FILE
#
# Sorties :
#	Les fichiers modifies par rapport a la base CVS
#	Les fichiers non tage
#	La liste des fichiers pour chaque tag (Mis dans des fichiers)
#-------------------------------------------------------------------------------
function dirname(path)
{
  return substr(path, 1, match(path, /\/[^\/]+$/)-1)
}

BEGIN {
	compteurFichier = 0
	compteurTag = 0
	erreurTag = 0
	if(output == "") { output = "/tmp" }
}

#
# Debut du corps
#
{
	#
	# ligne qui signale le debut les infos CVS
	#
	if ($1 == "===================================================================") {
		NumLigne = 1
		compteurFichier ++
		VersionDernierTag = ""
	}
	else {
		if (($1 == "cvs") && ($2 == "status:")) {
			NumLigne = 0
			dir = $4"/"
		}
	}

	#
	# Nom et etat du fichier
	#
	if (NumLigne == 2) {
		nomFichier = $2
		if (($4 == "Modified") || ($5 == "Modified")) {
			nokTag[compteurTag]++
			getline
			getline
			getline
			split($4, tmp, ",v")
			nomFichier = tmp[1]
			printf "%-80s \tfichier en edition, version = %s\n", nomFichier, $3
			erreurTag = 1
			NumLigne = 12
		}
	}

	#
	# Version CVS en cours d'utilisation
	#
	if (NumLigne == 4) {
		VersionTravail = $3
	}

	if (NumLigne == 5) {
		split($4, tmp, ",v")
		nomFichier = tmp[1]
	}

	#
	# Derniere version CVS tagee, ou Sticky Tag
	#
	if (NumLigne == 6 && $3 != "(none)") {
		StickedTag = 1
		TagCourant = $3
		VersionDernierTag = $5
	}

	if (NumLigne == 11) {
		if(VersionDernierTag == "") {
		        StickedTag = 0
		        TagCourant = $1
			VersionDernierTag = $3
		}
		longueur = length(VersionDernierTag)
		VersionDernierTag = substr(VersionDernierTag,0,longueur-1)
		if (VersionTravail != VersionDernierTag) {
			nokTag[compteurTag]++
			if (($1 == "No") && ($2 == "Tags") && ($3 == "Exist")) {
				printf "%-80s \tfichier sans TAG, version courante = %s\n", nomFichier, VersionTravail
				erreurTag = 1
			}
			else {
				printf "%-80s \tTAG (%s = %s), version courante = %s\n", nomFichier, $1, VersionDernierTag, VersionTravail
				erreurTag = 1
			}
		}
		else {
			if (compteurTag == 0) {
				compteurTag = 1
				tag[1] = TagCourant
				stickTag[1] = StickedTag
				numTag[1] = 1
				dirTag[1] = dirname(nomFichier)
				filename_output[1] = output"/"TagCourant".tag"
				printf "%-80s \t%s\n", nomFichier, VersionTravail > filename_output[1]
				close(filename_output[1])
			}
			else {
				newTag = 1
				for (i = 1; i <= compteurTag; i++) {
					if (tag[i] == TagCourant) {
						newTag = 0
						numTag[i] ++	
						break				
					}
				}
				if (newTag == 1) {
					compteurTag ++
					tag[compteurTag] = TagCourant
					stickTag[compteurTag] = StickedTag
					numTag[compteurTag] = 1
					nokTag[compteurTag] = 0
				        dirTag[compteurTag] = dirname(nomFichier)
					filename_output[compteurTag] = output"/"TagCourant".tag"
					printf "%-80s \t%s\n", nomFichier, VersionTravail > filename_output[compteurTag]
					close(filename_output[compteurTag])
				}
				else {
					printf "%-80s \t%s\n", nomFichier, VersionTravail > filename_output[i]
					close(filename_output[i])
				}
			}
		}
	}

	NumLigne ++
}

END {
	printf "\n--------------------------------------------\n"
	printf "\t%d fichiers traités\n", compteurFichier
	printf "--------------------------------------------\n\n"
	for (i = 1; i <= compteurTag; i++) {
		printf "%-15s %-40s (%d fichiers", tag[i], dirTag[i], numTag[i]
		if(nokTag[i] != 0) { printf " dont %d NOK", nokTag[i] }
		else { printf " OK" }
		if(stickTag[i] != 0) { printf ", sticked" }
		printf ")\n"
	}
	printf "\n--------------------------------------------\n"
	if(erreurTag) {
	  exit(-1)
        }
	else {
	  exit(0)
	}

}
