import axios from "axios";

export default function ping(url: string) : Promise<any> {
    url += "/ping";

    return new Promise<any>((resolve: (result: any) => void, reject) => {
        axios.get(url)
            .then(resolve)
            .catch(reject);
    });
}