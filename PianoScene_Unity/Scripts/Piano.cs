using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;

[System.Serializable]
public class Piano : MonoBehaviour
{

    //Dictionary<string, PianoKey> pianoDictionary; //Stores "C1 , PianoKeyAssociated"
    [HideInInspector]
    private List<PianoKey> keyList; //List of PianoKey objects in the game

    [Header("Library of Sounds")]
    public List<AudioClip> clips;

    //Coordinates from which Piano is generated (from left to right)
    private Vector3 originCoordinates;

    public int numberOfKeys = 12;
    public static Piano _instance;

    public static string[] noteNames = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };

    //Preceeding string to the name of the key for differentiating it from a text object
    [Header("Object & Name Settings")]
    public string _3dObjName = "key3D:";
    public string _txtPObjName = "txtPro: ";

    [Header("Text Configuration Properties:")]
    public GameObject textOverWhiteTilePrefab;
    public GameObject textOverBlackTilePrefab;
    public TMP_FontAsset fontAsset;
    public float fontSize = 20;
    public Color hexColor = new Color(1,1,1) ;

    [Header("Animation Configuration")]
    public Material pushedKeyMaterial;
    public Material blackKeys_Mat;
    public Material whiteKeys_Mat;

 

    public void GeneratePiano(int numberOfKeys)
    {
        keyList = new List<PianoKey>();
        Debug.Log("<b>---Piano Generated Successfully---</b>" +
                   "\n---<i>Number of Keys:</i> " + numberOfKeys + " ---");

        originCoordinates = gameObject.transform.position;


        //We create a static instance so that we can access methods of Piano from other scripts
        if (_instance == null)
            //if not, set instance to this
            _instance = this;
        if (_instance != this) { GameObject.Destroy(gameObject); }

        DontDestroyOnLoad(gameObject);


        # region Prefab Loading
        if (numberOfKeys < 0 || numberOfKeys > 81)
        {
            Debug.LogWarning("A " + numberOfKeys + " piano couldn't be generated");
            return;
        }
        GameObject newWhiteKeyPrefab = (GameObject)Resources.Load("Prefabs/PianoWhiteKey", typeof(GameObject));
        if (newWhiteKeyPrefab == null)
        {
            Debug.LogWarning("White Key Prefab NOT Found!");
            return;
        }
        GameObject newBlackKeyPrefab = (GameObject)Resources.Load("Prefabs/PianoBlackKey", typeof(GameObject));
        if (newBlackKeyPrefab == null)
        {
            Debug.LogWarning("Black Key Prefab NOT Found!");
            return;
        }
        if (textOverWhiteTilePrefab == null) //If prefab not provided from inspector, look for it in resources
            textOverWhiteTilePrefab = (GameObject)Resources.Load("Prefabs/TextCanvasWhiteKeys", typeof(GameObject));
        if (textOverBlackTilePrefab == null)
            textOverBlackTilePrefab = (GameObject)Resources.Load("Prefabs/TextCanvasBlackKeys", typeof(GameObject));
        #endregion

        Vector3 newPos = originCoordinates;

        int octave = 0;
        int clipIt = 0;

        //Creates an instance of each new key and assigns it a new position; finally, those get added to the piano key Lists
        for (int i = 0; i < numberOfKeys; i++)
        {
            GameObject newlyCreatedKey = null;
            GameObject newlyCreatedText = null;
            AudioSource newAudioOrigin = null; 

            int remainder = i % 12;
            bool keyType_ = false;
            Debug.Log("<color=green><b>PROC:</b></color> <i>Remainder</i>=" + remainder);

            switch (remainder)
            {
                case 0:  //WHITE C
                case 2:  //WHITE D
                case 4:  //WHITE E
                case 5:  //WHITE F
                case 7:  //WHITE G
                case 9:  //WHITE A
                case 11: //WHITE B
                    newlyCreatedKey = Instantiate(newWhiteKeyPrefab); keyType_ = false;
                    newlyCreatedText = Instantiate(textOverWhiteTilePrefab);
                    break;
                case 1: //BLACK
                case 3: //BLACK
                case 6: //BLACK
                case 8: //BLACK
                case 10: //BLACK
                    newlyCreatedKey = Instantiate(newBlackKeyPrefab); keyType_ = true;
                    newlyCreatedText = Instantiate(textOverBlackTilePrefab);
                    break;
                default:
                    Debug.Log("BAD INSTANCE ATTEMPT");
                    break;

            }
            //We update the value for octave: the octave we are currently generating, an integer number from #numKeys/12 notes
            octave = (i / 12);

            //We change the name of the new PianoKey object,  to one of the form "key3D C0" and set it to active and we also set the parent to this object
            newlyCreatedKey.name = _3dObjName + noteNames[remainder] + octave.ToString();
            newlyCreatedKey.SetActive(true);
            //Set the 3Dkey object parent to the object this script is assigned to. 
            newlyCreatedKey.transform.SetParent(gameObject.transform, false);

            //Update spawning location for each new iteration
            if (i != 0) newPos = newPos + new Vector3(5f * 1.14f, 0, 0);
            newlyCreatedKey.transform.position = newPos;

            //set Canvas+TextMeshProObject as children of gameObject
            newlyCreatedText.transform.SetParent(newlyCreatedKey.transform, false/*Check*/);
            //newlyCreatedText.gameObject.transform.GetChild(0).gameObject.GetComponent<TextMeshPro>().text="s" ;

            //Debug.Log("????--" + newlyCreatedText.name + "---????");

            //Debug.Log("????--" + newlyCreatedText.transform.GetChild(0).name + "---????");
            //Debug.Log("????--" + newlyCreatedText.transform.GetChild(0).gameObject + "---????");
            //newlyCreatedText.transform.GetChild(0).gameObject.GetComponent<TextMeshProUGUI>().text = noteNames[remainder];
            //obj.SetText("M");
            
            if(i >= clips.Count)
            {
                clipIt = 0;
            }

            //Create audio origin associated with a given audio clip
            newAudioOrigin = newlyCreatedKey.AddComponent<AudioSource>();

            //Create pianoKey Object associated to the actual 3D object
            PianoKey brandNewPianoKey = new PianoKey(noteNames[remainder],octave, keyType_, newlyCreatedKey, newlyCreatedText, clips[clipIt], newAudioOrigin);
            Debug.Log("<color=green><b>PROC:</b></color> " + brandNewPianoKey.noteName + " <color=teal>SUCCESSFULLY CREATED</color>");

            clipIt++;
            keyList.Add(brandNewPianoKey);

        }

    }
    public void setMaterials(Material whiteMaterial, Material blackMaterial, Material pushedMaterial)
    {
        blackKeys_Mat = blackMaterial;
        whiteKeys_Mat = whiteMaterial;
        pushedKeyMaterial = pushedMaterial;
    }
   

    public PianoKey getPianoKey(int numKey)
    {
        if (numKey >= keyList.Count)
        {
            Debug.LogError("<color=red><b>ERROR:</b></color> <i>numKey</i> is " + numKey + "; >" + keyList.Count);
            return null;
        }
        return keyList[numKey];
    }

    public bool playKey(int numKey, float volume)//Triggers playing the note and
    {
        if(numKey<0 || numKey> 127 || numKey >= keyList.Count) { Debug.Log("<color=red><b>ERROR:</b></color> Unable to <i>play</i> note #"+ numKey); return false; }
        keyList[numKey].Play(volume);//Trigger sound
        //Start animation associated to sound
        animate(numKey); 
       // keyList[numKey].associated3DObject.GetComponent<Renderer>().material = ;
        return true;
    }

    public bool shutKey(int numKey)
    {
        if (numKey < 0 || numKey > 127 || numKey >= keyList.Count) { Debug.Log("<color=red><b>ERROR:</b></color> Unable to <i>shut</i> note #" + numKey); return false; }
        keyList[numKey].Shut();
        resetAnimate(numKey);
        return true;

    }


   


    private void  animate(int numKey)
    {
        keyList[numKey].associated3DObject.GetComponent<Renderer>().material = pushedKeyMaterial;
    }

    private void resetAnimate(int numKey)
    {
        if (keyList[numKey].isWhiteKey())
        {
            keyList[numKey].associated3DObject.GetComponent<Renderer>().material = whiteKeys_Mat;
        }
        else
        {
            keyList[numKey].associated3DObject.GetComponent<Renderer>().material = blackKeys_Mat;

        }

        
    }


}
