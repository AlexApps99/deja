class PbDecoder {
    msg;
    reader;
    constructor(msg) {
        this.msg = msg;
        this.reader = new protobuf.Reader([]);
    }
    // Add new data to buf
    add(data) {
        this.reader.buf.push(...data);
        this.reader.len = this.reader.buf.length;
    }
    // Decode message
    dec() {
        let m = null;
        let old_pos = this.reader.pos;
        try {
            m = this.msg.decodeDelimited(this.reader);
        } catch (e) {
            console.warn(e);
        }
        console.log(this.reader.buf.slice(old_pos, this.reader.pos));
        this.reader.buf.splice(0, m != null ? this.reader.pos : old_pos);
        this.reader.pos = 0;
        this.reader.len = this.reader.buf.length;
        return m;
    }
    transform(chunk, controller) {
        this.add(chunk);
        let d = this.dec();
        if (d) controller.enqueue(d);
    }
    flush(controller) { }
}

class Comm {
    root;
    Ground;
    Drone;
    io;
    decoder;
    constructor(serialport, pb_root, msg_callback) {
        this.io = serialport;
        this.root = pb_root;
        this.Ground = this.root.lookupType("Ground");
        this.Drone = this.root.lookupType("Drone");
        this.io.onconnect = (e) => { console.log(e); };
        this.io.ondisconnect = (e) => { console.log(e); };
        this.decoder = new TransformStream(new PbDecoder(Drone));
        this.io.readable.pipeThrough(this.decoder);
        console.log(this.decoder);

        (async () => {
            const r = this.decoder.readable.getReader();
            while (1) {
                try {
                    let msg = (await r.read()).value;
                    if (msg.hasOwnProperty("log")) {
                        switch (msg.log.level) {
                            case 3:
                                console.debug(msg.log.text);
                                break;
                            case 2:
                                console.info(msg.log.text);
                                break;
                            case 1:
                                console.warn(msg.log.text);
                                break;
                            case 0:
                            default:
                                console.error(msg.log.text);
                                break;
                        }
                    } else {
                        console.log(msg);
                    }
                } catch (e) { console.log(e); break; }
            }
        })();
    }

    send(...msgs) {
        const encs = msgs.map((m) => {
            let ver = Ground.verify(m);
            if (ver) {
                console.warn(ver);
                return null;
            } else {
                const msg = Ground.create(m);
                return this.Ground.encodeDelimited(msg).finish();
            }
        });
        const w = this.io.writable.getWriter();
        for (const enc of encs) {
            if (enc) w.write(enc);
        }
        w.releaseLock();
    }
}

export default async function main() {
    const choose = document.getElementById("choose");
    const root = await protobuf.load("../msg.proto");
    globalThis.Ground = root.lookupType("Ground");
    globalThis.Drone = root.lookupType("Drone");
    globalThis.pbd = new PbDecoder(globalThis.Drone);
    globalThis.tf = new TransformStream(globalThis.pbd);
    choose.addEventListener("click", async (el, ev) => {
        const serial = await navigator.serial.requestPort({
            filters: [{
                usbVendorId: 0x2E8A,
                usbProductId: 0x000A
            }]
        });
        if (!serial) return;
        await serial.open({
            baudRate: 115200,
            bufferSize: 512 * 1024,
        });
        let comm = new Comm(serial, root);
    });
}