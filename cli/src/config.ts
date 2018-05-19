export const CONFIG_FILE_NAME = ".neve";

export class Config {
    readonly url: string;

    constructor(json: string) {
        const parsed = JSON.parse(json);
        this.url = parsed["url"];
    }
}