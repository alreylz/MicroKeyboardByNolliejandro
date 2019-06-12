using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MidiInController : MonoBehaviour {

    //public short listening_MIDIChannel = 16; // 0-15 , 16 means listen to all channels

    [Header("Animation Configuration")]
    public Material blackKeys_Mat;
    public Material whiteKeys_Mat;
    public Material pushedKeyMaterial;
    



    //Animates a key as pushed, changing its color 
    public void animateNoteOn(MidiJack.MidiChannel channel, int note, float velocity) {
        //Backup material to restore on noteOff
        Piano._instance.playKey(note, velocity);
       // plainKeyMaterial[note] = Piano._instance.getPianoKey(note).associated3DObject.GetComponent<Renderer>().material;
        //Change material to defined one
       // Piano._instance.getPianoKey(note).associated3DObject.GetComponent<Renderer>().material = pushedKeyMaterial;
    }
    //Key goes back to the original status
    public void animateNoteOff(MidiJack.MidiChannel channel, int note)
    {
        Piano._instance.shutKey(note);
       // Piano._instance.getPianoKey(note).associated3DObject.GetComponent<Renderer>().material = plainKeyMaterial[note];
    }



    void Awake () {
        //plainKeyMaterial = new Material[128];
        Piano piano_ = FindObjectOfType<Piano>();
        piano_.GeneratePiano(12);
        piano_.setMaterials(whiteKeys_Mat, blackKeys_Mat, pushedKeyMaterial);

        MidiJack.MidiMaster.noteOnDelegate = animateNoteOn;
        MidiJack.MidiMaster.noteOffDelegate = animateNoteOff;
    }
	

}
