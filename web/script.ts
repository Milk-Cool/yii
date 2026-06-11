const connect = document.querySelector("#connect") as HTMLButtonElement;
const config = document.querySelector("#config") as HTMLDivElement; config.style.display = "none";
const remote = document.querySelector("#remote") as HTMLDivElement; remote.style.display = "none";
const namel = document.querySelector("#name") as HTMLInputElement;
const password = document.querySelector("#password") as HTMLInputElement;
const save = document.querySelector("#save") as HTMLButtonElement;

const encoder = new TextEncoder();
const decoder = new TextDecoder();

let writer: WritableStreamDefaultWriter, reader: ReadableStreamDefaultReader;
const wint = async (a: number[]) => await writer.write(new Uint8Array(a).buffer);
const wu8a = async (a: Uint8Array) => await writer.write(a.buffer);

connect.addEventListener("click", async () => {
    try {
        // @ts-ignore // too lazy to figure out how to install types
        const port = await navigator.serial.requestPort();
        await port.open({ baudRate: 115200 });
        reader = (port.readable as ReadableStream).getReader();
        writer = (port.writable as WritableStream).getWriter();

        if(confirm("OK to load the remote interface, Cancel to load the config interface")) {

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