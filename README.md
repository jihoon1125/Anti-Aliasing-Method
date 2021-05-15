# Anti-Aliasing-Method
Anti-aliasing 필터를 구현하고, 이미지 압축 및 복원 결과를 필터를 적용하기 전후로 비교하며 분석한다

---

## Project 설명

* 이미지 Downsampling 이전에 anti-alising filter를 적용하면 upsampling 한 결과에 영향을 얼마나 미칠지 확인한다.

* 이미지의 각 픽셀값들을 이산적인 신호로 생각하면 충분히 신호처리 개념을 적용할 수 있다.

![image](https://user-images.githubusercontent.com/67624104/118352258-6987d500-b59b-11eb-941a-ad0122efcf95.png)


* Continuous, analog domain의 자연 형상들을 카메라로 샘플링하면 왼쪽 그림의 세번째 그래프같은 형태로 주파수 영역이 도출된다.

* 여기서 카메라의 Sampling frequency가 낮아서 애초부터 aliasing이 일어나는 case는 고려하지 않았다.

* Downsampling을 하게 되면 주파수 간의 간격이 더 좁아진다.

* 간격이 좁아져서 aliasing이 일어났을 때의 피해를 최소화 하기 위해 사전에 high frequency 영역을 주기적인 rectangular wave로 잘라내야 한다.

* 주파수 영역에서의 사각파 => 시간 영역에서의 이산 sync함수, 즉 이것이 Practical하게 Gaussian filter를 사용하는 원리이다.

* High frequency 영역의 빈도가 많을수록 anti-alising 필터 적용의 의미가 커질 것이다.
---

## Result & Analysis

* 예시 1

![image](https://user-images.githubusercontent.com/67624104/118352442-6b9e6380-b59c-11eb-8758-a861dfb5f7b2.png)

원본 이미지

![image](https://user-images.githubusercontent.com/67624104/118352453-7ce77000-b59c-11eb-9f93-979072e4b24e.png)

Downsample, upsample된 이미지(anti-aliasing ver)

![image](https://user-images.githubusercontent.com/67624104/118352462-8a9cf580-b59c-11eb-911f-c672c7e68fdd.png)

![image](https://user-images.githubusercontent.com/67624104/118352531-e9626f00-b59c-11eb-9689-a5e425853429.png)


Downsample, upsample된 이미지(Non anti-aliasing ver)

![image](https://user-images.githubusercontent.com/67624104/118352472-97b9e480-b59c-11eb-96ca-38b3d300a26f.png)

![image](https://user-images.githubusercontent.com/67624104/118352534-ef585000-b59c-11eb-92fe-36e8cfb536d0.png)





* 예시 2

![image](https://user-images.githubusercontent.com/67624104/118352505-ca63dd00-b59c-11eb-8e46-22b3ce5096fc.png)


원본 이미지

![image](https://user-images.githubusercontent.com/67624104/118352511-d059be00-b59c-11eb-921f-5181eed0c17e.png)

Downsample, upsample된 이미지(anti-aliasing ver)

![image](https://user-images.githubusercontent.com/67624104/118352517-d51e7200-b59c-11eb-9d44-e346f7ffdce1.png)

![image](https://user-images.githubusercontent.com/67624104/118352540-f67f5e00-b59c-11eb-940c-da187b14f4d5.png)


Downsample, upsample된 이미지(Non anti-aliasing ver)

![image](https://user-images.githubusercontent.com/67624104/118352525-da7bbc80-b59c-11eb-8bb3-d864faa28110.png)

![image](https://user-images.githubusercontent.com/67624104/118352560-1747b380-b59d-11eb-8d6c-d14e7f4c4324.png)


* 확실히 예시 2의 경우에는 고주파 영역의 aliasing 현상이 크게 일어나는 것이 보이기 때문에 확연한 차이를 느낄 수가 있다.
