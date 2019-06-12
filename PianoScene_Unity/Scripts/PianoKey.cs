using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;

[System.Serializable]
public class PianoKey {

    public GameObject associated3DObject; //3D rectangle associated with the key
    public GameObject associatedCanvasObject; //Associated canvas where the note name appears
    public TextMeshProUGUI associatedTextComponent; //Text itself 

    public string wholeNoteName; //Name for the note, including octave number (i.e. "C#0" ) 
    public string noteName;
    public int octave;

    public string obj3DName ; //Name of the 3DObject that appears in the hierarchy
    public string CanvasName; //Name of the Canvas object that appears in the hierarchy

    public bool keyType; //false is white, true is black

    public AudioClip audioFile; //File to be heard on press
    public AudioSource audioOriginComponent; //Origin of 3D sound 

    public static string _3dObjNamePrefix = "key3D: ";
    public static string _canvasObjNamePrefix = "txtPro: ";

    const int defaultFontSize = 8;

    public PianoKey(string plainNoteName, int octave_, bool keyType_, GameObject gO3D, GameObject tmshp_, AudioClip audioClip, AudioSource audioSrcComponent)
    {
        //Set references to 3d object and canvas associated to "this" key
        associated3DObject = gO3D;
        associatedCanvasObject = tmshp_;
        keyType = keyType_;
        noteName = plainNoteName;
        octave = octave_;
        wholeNoteName = plainNoteName + octave.ToString();

        //Modify the name of the Piano key 3D Object so that it follows a naming convention
        associated3DObject.name = _3dObjNamePrefix + noteName;
        tmshp_.name = _canvasObjNamePrefix + CanvasName;
        //Set text of the TextMeshPro Component to the name of the note (i.e. C)
        associatedTextComponent = tmshp_.transform.GetChild(0).gameObject.GetComponent<TextMeshProUGUI>();
        associatedTextComponent.text = noteName;
        associatedTextComponent.fontSize = defaultFontSize;
        //Set audio Clip to keep a reference just in case I need it [REVISIT]
        audioFile = audioClip;
        //Set reference within audioSource component
        audioOriginComponent = audioSrcComponent;
        audioOriginComponent.volume = 1f; //[REVISIT]
        audioOriginComponent.loop = true;
        audioOriginComponent.clip = audioClip;


    }

    bool set3DObjPrefix(string prefix)
    {
        if(prefix == null) { return false; }
        _3dObjNamePrefix = prefix;
        if (_3dObjNamePrefix.Equals(prefix)) return true;
        return false;
    }
    bool setCanvasObjPrefix(string prefix)
    {
        if (prefix == null) { return false; }
        _canvasObjNamePrefix = prefix;
        if (_canvasObjNamePrefix.Equals(prefix)) return true;
        return false;
    }

    public void Play(float volume){
        audioOriginComponent.volume = volume;
        audioOriginComponent.Play();
    }
    public void Shut()
    {
        if (audioOriginComponent.isPlaying)
        {
            audioOriginComponent.Pause();
        }
    }
  
    public bool isWhiteKey()
    {
        if(keyType == false)
        {
            return true;
        }
        return false;
    }

    public float getFontSize()
    {
        return associatedTextComponent.fontSize;
    }

    public void setFontSize(float size)
    {
        associatedTextComponent.fontSize = size;
    }
    public void setFontColor(Color faceColor){
        associatedTextComponent.faceColor = faceColor;
    }
    public Color getFontColor()
    {
        return associatedTextComponent.faceColor;
    }

}
