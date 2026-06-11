const connect = document.querySelector("#connect") as HTMLButtonElement;
const config = document.querySelector("#config") as HTMLDivElement; config.style.display = "none";
const remote = document.querySelector("#remote") as HTMLDivElement; remote.style.display = "none";
const namel = document.querySelector("#name") as HTMLInputElement;
const password = document.querySelector("#password") as HTMLInputElement;
const save = document.querySelector("#save") as HTMLButtonElement;
const devices = document.querySelector("#devices") as HTMLSelectElement;
const feed = document.querySelector("#feed") as HTMLButtonElement;
const poke = document.querySelector("#poke") as HTMLButtonElement;
const playRps = document.querySelector("#play-rps") as HTMLButtonElement;
const rps = document.querySelector("#rps") as HTMLDivElement; rps.style.display = "none";
const rock = document.querySelector("#rock") as HTMLButtonElement;
const paper = document.querySelector("#paper") as HTMLButtonElement;
const scissors = document.querySelector("#scissors") as HTMLButtonElement;

const encoder = new TextEncoder();
const decoder = new TextDecoder();

let writer: WritableStreamDefaultWriter, reader: ReadableStreamDefaultReader;
const wint = async (a: number[]) => await writer.write(new Uint8Array(a).buffer);
const wu8a = async (a: Uint8Array) => await writer.write(a.buffer);

const strmac = (mac: number[]) => mac.map(x => x.toString(16).padStart(2, "0")).join("-");
const devlist: Record<string, { name: string, xp: number, level: number, saturation: number }> = {};
let rxbuf = "";

const txact = async (n: number) => {
    const mac = devices.value.split("-").map(x => parseInt(x, 16));
    await writer.write(encoder.encode(JSON.stringify({ mac, action: n }) + "\n").buffer);
};
feed.addEventListener("click", () => txact(0));
poke.addEventListener("click", () => txact(1));
playRps.addEventListener("click", () => {
    txact(2);
    rps.style.display = "";
});
rock.addEventListener("click", () => txact(3));
paper.addEventListener("click", () => txact(4));
scissors.addEventListener("click", () => txact(5));

devices.addEventListener("change", () => rps.style.display = "none");

connect.addEventListener("click", async () => {
    try {
        // @ts-ignore // too lazy to figure out how to install types
        const port = await navigator.serial.requestPort();
        await port.open({ baudRate: 115200 });
        reader = (port.readable as ReadableStream).getReader();
        writer = (port.writable as WritableStream).getWriter();

        if(confirm("OK to load the remote interface, Cancel to load the config interface")) {
            remote.style.display = "";
            setInterval(async () => {
                const r = await reader.read();
                rxbuf += decoder.decode(r.value as Uint8Array);
                while(rxbuf.includes("\n")) {
                    const idx = rxbuf.indexOf("\n");
                    const jsonStr = rxbuf.slice(0, idx);
                    rxbuf = rxbuf.slice(idx + 1);
                    if(!jsonStr.length || jsonStr[0] !== "{") continue;
                    try {
                        const json = JSON.parse(jsonStr);
                        const m = strmac(json.mac);
                        if(!devlist[m]) {
                            devlist[m] = { name: "", xp: 0, level: 0, saturation: 0 };
                            
                            const el = document.createElement("option");
                            el.id = m;
                            el.value = m;
                            devices.appendChild(el);
                        }
                        devlist[m].name = json.name;
                        devlist[m].xp = json.xp;
                        devlist[m].level = json.level;
                        devlist[m].saturation = json.saturation;
                        document.getElementById(m)!.innerText = `${json.name} | ${json.level}L ${json.xp}XP, ${json.saturation} saturation`;
                    } catch(_) {}
                }
            })
        } else {
            config.style.display = "";
            await wint([0x59, 0x44, 0x54, 0x91, 0xff, 0x21, 0x73, 0x65, 0x74, 0x75, 0x70, 0x5f, 0x77, 0x72, 0x69, 0x74, 0x65, 0x00, 0x01, 0x00]);
            await wint([0xff]); await wu8a(encoder.encode("name")); await wint([0x00, 0x08, 0x00]);
            await wint([0xff]); await wu8a(encoder.encode("password")); await wint([0x00, 0x08, 0x00]);
            await wint([0x00]);
            let d = new Uint8Array();
            const interval = setInterval(async () => {
                const r = await reader.read();
                const v = r.value as Uint8Array;
                console.log(...v);
                d = Uint8Array.from([...d, ...v]);
                while(d.length >= 4 && !(d[0] === 0x59 && d[1] === 0x44 && d[2] === 0x54 && d[3] === 0x91))
                    d = d.subarray(1);

                let dl = d.subarray(4);
                let kl = "", vl = "";
                const obj: Record<string, string> = {};
                while(true) {
                    if(dl[0] === undefined) return;
                    if(dl[0] === 0x00) break;
                    if(dl[0] === 0x08 || dl[0] === 0xff) {
                        let t = dl[0];
                        dl = dl.subarray(1);
                        if(dl[0] === undefined) return;
                        let str = "";
                        while(dl[0] !== 0x00) {
                            str += String.fromCharCode(dl[0]);
                            dl = dl.subarray(1);
                            if(dl[0] === undefined) return;
                        }
                        dl = dl.subarray(1);
                        if(dl[0] === undefined) return;
                        if(t === 0x08) vl = str;
                        else kl = str;
                        obj[kl] = vl;
                        continue;
                    }
                    return;
                }
                clearInterval(interval);
                namel.value = obj.name;
                password.value = obj.password;
            }, 10);
        }
    } catch(e) {
        alert(e);
        console.error(e);
    }
});

save.addEventListener("click", async () => {
    await wint([0x59, 0x44, 0x54, 0x91, 0xff, 0x21, 0x73, 0x65, 0x74, 0x75, 0x70, 0x5f, 0x77, 0x72, 0x69, 0x74, 0x65, 0x00, 0x01, 0x01]);
    await wint([0xff]); await wu8a(encoder.encode("name")); await wint([0x00, 0x08]); await wu8a(encoder.encode(namel.value)); await wint([0x00]);
    await wint([0xff]); await wu8a(encoder.encode("password")); await wint([0x00, 0x08]); await wu8a(encoder.encode(password.value)); await wint([0x00]);
    await wint([0x00]);
    alert("success!!");
})